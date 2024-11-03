#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/util.h>
#include <vcmp/socks5.h>

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/log.h"

static int socks5_send_req(struct bufferevent *bev, socks5_ctx_t *ctx)
{
    uint8_t *buf = NULL;
    int buflen = 0;

    if (ctx->ipv4_addr) {
        buflen = 10;
        buf = (uint8_t *)malloc(buflen);
        if (!buf) {
            log_fatal("Failed to alloc buf");
            return 1;
        }
        buf[3] = SOCKS5_ATYP_IPV4;
        memcpy(buf + 4, ctx->ipv4_addr, 4);
        uint16_t p = htons(ctx->dst_port);
        memcpy(buf + 8, &p, 2);
    }
    else if (ctx->hostname) {
        buflen = ctx->hostname->len + 7;
        buf = (uint8_t *)malloc(buflen);
        if (!buf) {
            log_fatal("Failed to alloc buf");
            return 1;
        }
        buf[3] = SOCKS5_ATYP_DOMAINNAME;
        buf[4] = ctx->hostname->len;
        memcpy(buf + 5, ctx->hostname->hostname, ctx->hostname->len);
        uint16_t p = htons(ctx->dst_port);
        memcpy(buf + ctx->hostname->len + 5, &p, 2);
    }

    buf[0] = SOCKS5_VERSION;
    buf[1] = SOCKS5_CMD_CONNECT;
    buf[2] = SOCKS5_RESERVED;

    int ret = bufferevent_write(bev, buf, buflen);
    free(buf);

    if (ret != 0) {
        log_warn("Failed to send request packet");
        return 1;
    }

    return 0;
}

static void read_cb(struct bufferevent *bev, void *ctx)
{
    socks5_ctx_t *s5_ctx = (socks5_ctx_t *)ctx;

    struct evbuffer *input = bufferevent_get_input(bev);
    int len = evbuffer_get_length(input);

    struct evbuffer_iovec vec[1];

    switch (s5_ctx->stage) {
    case SOCKS5_STAGE_IDENT: {
        evbuffer_peek(input, 2, NULL, vec, 1);
        uint8_t *data = (uint8_t *)vec[0].iov_base;

        uint8_t ver = data[0];
        uint8_t method = data[1];

        if (ver != SOCKS5_VERSION) {
            log_debug("Version was not SOCKS5");
            if (s5_ctx->eventcb)
                s5_ctx->eventcb(bev, BEV_EVENT_ERROR, s5_ctx->ctx);
            bufferevent_free(bev);
            socks5_ctx_free(s5_ctx);
            return;
        }

        if (method == SOCKS5_METHOD_NONE) {
            log_debug("Chosen no authentication method");
            if (socks5_send_req(bev, s5_ctx) != 0) {
                log_warn("Failed to send request packet");
                if (s5_ctx->eventcb)
                    s5_ctx->eventcb(bev, BEV_EVENT_ERROR, s5_ctx->ctx);
                bufferevent_free(bev);
                socks5_ctx_free(s5_ctx);
                return;
            }
            s5_ctx->stage = SOCKS5_STAGE_REQUEST;
        }
        else {
            if (s5_ctx->eventcb)
                s5_ctx->eventcb(bev, BEV_EVENT_ERROR, s5_ctx->ctx);
            bufferevent_free(bev);
            socks5_ctx_free(s5_ctx);
            return;
        }

        evbuffer_drain(input, len);
        break;
    }
    case SOCKS5_STAGE_REQUEST: {
        evbuffer_peek(input, 2, NULL, vec, 1);
        uint8_t *data = (uint8_t *)vec[0].iov_base;

        if (data[0] != SOCKS5_VERSION || data[2] != SOCKS5_RESERVED) {
            if (s5_ctx->eventcb)
                s5_ctx->eventcb(bev, BEV_EVENT_ERROR, s5_ctx->ctx);
            bufferevent_free(bev);
            socks5_ctx_free(s5_ctx);
            return;
        }
        if (data[1] != SOCKS5_REPLY_SUCCEEDED) {
            if (s5_ctx->eventcb)
                s5_ctx->eventcb(bev, BEV_EVENT_ERROR, s5_ctx->ctx);
            bufferevent_free(bev);
            socks5_ctx_free(s5_ctx);
            return;
        }
        evbuffer_drain(input, len);

        s5_ctx->stage = SOCKS5_STAGE_CONNECTED;
        if (s5_ctx->eventcb)
            s5_ctx->eventcb(bev, BEV_EVENT_CONNECTED, s5_ctx->ctx);
        break;
    }
    case SOCKS5_STAGE_CONNECTED: {
        if (s5_ctx->readcb)
            s5_ctx->readcb(bev, s5_ctx->ctx);
        break;
    }
    }
}

static void write_cb(struct bufferevent *bev, void *ctx)
{
    socks5_ctx_t *s5_ctx = (socks5_ctx_t *)ctx;
    if (s5_ctx->stage == SOCKS5_STAGE_CONNECTED && s5_ctx->writecb)
        s5_ctx->writecb(bev, s5_ctx->ctx);
}

static void event_cb(struct bufferevent *bev, short events, void *ctx)
{
    socks5_ctx_t *s5_ctx = (socks5_ctx_t *)ctx;

    if (events & BEV_EVENT_CONNECTED) {
        uint8_t ident[3];
        ident[0] = SOCKS5_VERSION;
        ident[1] = 1;
        ident[2] = SOCKS5_METHOD_NONE;

        if (bufferevent_write(bev, ident, sizeof(ident)) != 0) {
            log_warn("Failed to send ident packet");
            if (s5_ctx->eventcb)
                s5_ctx->eventcb(bev, BEV_EVENT_ERROR, s5_ctx->ctx);
            bufferevent_free(bev);
            socks5_ctx_free(s5_ctx);
            return;
        }

        s5_ctx->stage = SOCKS5_STAGE_IDENT;
    }
    else if (events & (BEV_EVENT_ERROR | BEV_EVENT_EOF)) {
        if (s5_ctx->eventcb)
            s5_ctx->eventcb(bev, events, s5_ctx->ctx);
        socks5_ctx_free(s5_ctx);
    }
}

socks5_ctx_t *socks5_ctx_new_hostname(const char *dst_hostname,
                                      const uint16_t dst_port,
                                      socks5_read_cb readcb,
                                      socks5_write_cb writecb,
                                      socks5_event_cb eventcb, void *ctx)
{
    socks5_ctx_t *c = (socks5_ctx_t *)malloc(sizeof(socks5_ctx_t));
    if (!c)
        return NULL;

    socks5_hostname_t *hostname =
        (socks5_hostname_t *)malloc(sizeof(socks5_hostname_t));
    if (!hostname) {
        free(c);
        return NULL;
    }

    hostname->len = snprintf(hostname->hostname, sizeof(hostname->hostname),
                             "%s", dst_hostname);

    c->hostname = hostname;
    c->dst_port = dst_port;

    c->readcb = readcb;
    c->writecb = writecb;
    c->eventcb = eventcb;
    c->ctx = ctx;
    return c;
}

socks5_ctx_t *socks5_ctx_new_ipv4(const char *dst_addr, const uint16_t dst_port,
                                  socks5_read_cb readcb,
                                  socks5_write_cb writecb,
                                  socks5_event_cb eventcb, void *ctx)
{
    socks5_ctx_t *c = (socks5_ctx_t *)malloc(sizeof(socks5_ctx_t));
    if (!c)
        return NULL;

    unsigned int *ipv4 = (unsigned int *)malloc(sizeof(unsigned int));
    if (!ipv4) {
        free(c);
        return NULL;
    }

    if (evutil_inet_pton(AF_INET, dst_addr, ipv4) != 1) {
        log_error("inet pton failed");
        free(c);
        return NULL;
    }

    c->ipv4_addr = ipv4;
    c->dst_port = dst_port;

    c->readcb = readcb;
    c->writecb = writecb;
    c->eventcb = eventcb;
    c->ctx = ctx;
    return c;
}

void socks5_ctx_free(socks5_ctx_t *ctx)
{
    if (ctx->hostname)
        free(ctx->hostname);
    if (ctx->ipv4_addr)
        free(ctx->ipv4_addr);
    free(ctx);
}

int socks5_client_init(struct event_base *base, const char *proxy_ip,
                       const uint16_t proxy_port, socks5_ctx_t *ctx)
{
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(proxy_port);
    if (evutil_inet_pton(AF_INET, proxy_ip, &sin.sin_addr) != 1) {
        log_error("inet_pton failed");
        return 1;
    }

    struct bufferevent *bev =
        bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    if (!bev) {
        log_error("Failed to init a bev");
        return 1;
    }

    bufferevent_setcb(bev, read_cb, write_cb, event_cb, (void *)ctx);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) !=
        0) {
        bufferevent_free(bev);
        log_error("Failed to connect");
        return 1;
    }

    return 0;
}