#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/util.h>

#include <netinet/in.h>

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <stdlib.h>
#include <vcmp/listener.h>
#include <vcmp/protocol.h>
#include <vcmp/rsa.h>

#include "../lib/log.h"
#include <string.h>

#include <vcmp/util/randdata.h>

static EVP_PKEY *keypair;

static int vcmp_validate(vcmp_header_t *h)
{
    if (h->magic[0] != VCMP_MAGIC[0] || h->magic[1] != VCMP_MAGIC[1] ||
        h->magic[2] != VCMP_MAGIC[2] || h->magic[3] != VCMP_MAGIC[3] ||
        h->magic[4] != VCMP_MAGIC[4])
        return 1;

    if (h->version != VCMP_VERSION)
        return 1;

    return 0;
}

static void read_cb(struct bufferevent *bev, void *ctx)
{
    vcmp_peer_t *peer = (vcmp_peer_t *)ctx;

    log_info("Got some data from peer");
    struct evbuffer *input = bufferevent_get_input(bev);

    // Validate the peer client
    if (evbuffer_get_length(input) < sizeof(vcmp_header_t)) {
        log_error("Packet doesn't hold vcmp header");

        free(peer);
        bufferevent_free(bev);
        return;
    }

    switch (peer->stage) {
    case VCMP_PEER_STAGE_NONE: {
        // Validate client

        struct evbuffer_iovec vec[1];
        evbuffer_peek(input, sizeof(vcmp_header_t), NULL, vec, 1);
        vcmp_header_t *h = (vcmp_header_t *)vec[0].iov_base;

        if (vcmp_validate(h) != 0 || h->type != VCMP_TYPE_VERIFY) {
            log_error("Peer was not a valid client.");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        evbuffer_drain(input, sizeof(vcmp_header_t));

        // Send public key
        int keylen = 0;
        uint8_t *der_pubkey = rsa_get_der_pubkey(keypair, &keylen);

        if (!der_pubkey) {
            log_fatal("Could not export RSA public key in DER!");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        vcmp_handshake_t *hs =
            vcmp_gen_hs_header(VCMP_TYPE_HS_INIT, keylen, der_pubkey);
        OPENSSL_free(der_pubkey); // der_pubkey is copied into hs

        if (!hs) {
            log_fatal("Could not generate initial handshake header");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        int ret =
            bufferevent_write(bev, hs, sizeof(vcmp_handshake_t) + hs->keylen);
        free(hs);

        if (ret != 0) {
            log_fatal("Could not send initial handshake header to peer");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        peer->stage = VCMP_PEER_STAGE_PUBKEY_WAIT;
        break;
    }
    case VCMP_PEER_STAGE_PUBKEY_WAIT: {
        // Receive peer's public key

        int len = evbuffer_get_length(input);
        if (len <= sizeof(vcmp_handshake_t)) {
            log_error("Packet doesn't hold vcmp handshake struct");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        struct evbuffer_iovec vec[1];
        evbuffer_peek(input, sizeof(vcmp_handshake_t), NULL, vec, 1);
        vcmp_handshake_t *h = (vcmp_handshake_t *)vec[0].iov_base;

        if (len != sizeof(vcmp_handshake_t) + h->keylen) {
            log_error("Packet malformed");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        if (vcmp_validate(&h->header) != 0) {
            log_error("Peer was not a valid client.");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        if (h->header.type != VCMP_TYPE_HS_ACK) {
            log_error("Handshake type was not ACK");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        // Load public key of the peer
        EVP_PKEY *pubkey = rsa_load_der_pubkey(h->pubkey, h->keylen);
        evbuffer_drain(input, len);

        if (!pubkey) {
            log_error("Pubkey malformed");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        // Check public keys authenticity by sending a dummy vcmp_data struct
        // Send a random payload and expect it to be echoed by decrypting

        rand_array(peer->authenticity, sizeof(peer->authenticity));

        // encrypt rand_arr
        size_t enclen = 0;
        uint8_t *enc = rsa_encrypt((uint8_t *)peer->authenticity,
                                   sizeof(peer->authenticity), pubkey, &enclen);
        if (!enc) {
            log_error("Failed to encrypt a random array");
            EVP_PKEY_free(pubkey);
            free(peer);
            bufferevent_free(bev);
            return;
        }

        char *buf = (char *)malloc(sizeof(vcmp_data_t) + enclen);
        if (!buf) {
            log_error("Failed to alloc a buffer for encrypted random array");
            EVP_PKEY_free(pubkey);
            free(enc);
            free(peer);
            bufferevent_free(bev);
            return;
        }

        vcmp_data_t *d = (vcmp_data_t *)buf;
        vcmp_gen_data_header(d, enclen);
        memcpy(d->payload, enc, enclen);
        free(enc);

        int ret = bufferevent_write(bev, buf, sizeof(buf));
        free(buf);

        if (ret != 0) {
            log_error("Failed to send a fake public key authenticity packet");
            EVP_PKEY_free(pubkey);
            free(peer);
            bufferevent_free(bev);
            return;
        }

        peer->pubkey = pubkey;
        peer->stage = VCMP_PEER_STAGE_PUBKEY_CHECK;
        break;
    }
    case VCMP_PEER_STAGE_PUBKEY_CHECK: {
        // Verify that decrypted result is equal to peer->authenticity

        int len = evbuffer_get_length(input);
        if (len <= sizeof(vcmp_data_t)) {
            log_error("Packet doesn't hold vcmp data");
            free(peer);
            bufferevent_free(bev);
            return;
        }

        struct evbuffer_iovec vec[1];
        evbuffer_peek(input, sizeof(vcmp_data_t), NULL, vec, 1);
        vcmp_data_t *h = (vcmp_data_t *)vec[0].iov_base;

        if (len != sizeof(vcmp_data_t) + h->len) {
            log_error("Packet malformed");
            EVP_PKEY_free(peer->pubkey);
            free(peer);
            bufferevent_free(bev);
            return;
        }

        if (vcmp_validate(&h->header) != 0) {
            log_error("Peer was not a valid client.");
            EVP_PKEY_free(peer->pubkey);
            free(peer);
            bufferevent_free(bev);
            return;
        }

        if (h->header.type != VCMP_TYPE_DATA) {
            log_error("Authenticity packet is not data type");
            EVP_PKEY_free(peer->pubkey);
            free(peer);
            bufferevent_free(bev);
            return;
        }

        size_t declen = 0;
        uint8_t *dec = rsa_decrypt(h->payload, h->len, keypair, &declen);
        evbuffer_drain(input, len);

        if (!dec) {
            log_error("Failed to decrypt authenticity payload");
            EVP_PKEY_free(peer->pubkey);
            free(peer);
            bufferevent_free(bev);
            return;
        }

        int ret = memcmp(peer->authenticity, dec, sizeof(peer->authenticity));
        free(dec);

        if (ret != 0) {
            log_error("Peer failed to verify authenticity");
            EVP_PKEY_free(peer->pubkey);
            free(peer);
            bufferevent_free(bev);
            return;
        }

        log_info("Peer is ready to communicate with!");
        peer->stage = VCMP_PEER_STAGE_READY;
        break;
    }
    }
}

static void write_cb(struct bufferevent *bev, void *ctx)
{
    vcmp_peer_t *peer = (vcmp_peer_t *)ctx;
}

static void event_cb(struct bufferevent *bev, short events, void *ctx)
{
    vcmp_peer_t *peer = (vcmp_peer_t *)ctx;
    log_info("Got some event from peer");
}

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
                           struct sockaddr *address, int socklen, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev =
        bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    if (!bev) {
        log_warn("Failed to allocate a new bufferevent for peer");
        return;
    }

    vcmp_peer_t *peer = (vcmp_peer_t *)malloc(sizeof(vcmp_peer_t));
    if (!peer) {
        log_warn("Failed to allocate a new peer obj");
        return;
    }

    peer->pubkey = NULL;
    peer->stage = VCMP_PEER_STAGE_NONE;
    memset(peer->authenticity, 0, sizeof(peer->authenticity));

    bufferevent_setcb(bev, read_cb, write_cb, event_cb, (void *)peer);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    log_error("Got an error %d (%s) on the listener. "
              "Shutting down.\n",
              err, evutil_socket_error_to_string(err));

    event_base_loopexit(base, NULL);
}

int vcmp_listener_init(struct event_base *base, const char *ip,
                       const uint16_t port)
{
    // Generate a RSA key pair
    keypair = rsa_generate();
    if (!keypair) {
        log_error("Failed to generate a RSA key pair!");
        return 1;
    }

    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if (evutil_inet_pton(AF_INET, ip, &sin.sin_addr) != 1) {
        log_error("Failed to convert address to network order");
        return 1;
    }

    struct evconnlistener *listener = evconnlistener_new_bind(
        base, accept_conn_cb, NULL, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
        -1, (struct sockaddr *)&sin, sizeof(sin));

    if (!listener) {
        log_error("Couldn't create listener");
        return 1;
    }

    evconnlistener_set_error_cb(listener, accept_error_cb);
    return 0;
}