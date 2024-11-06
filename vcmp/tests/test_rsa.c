#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <stdint.h>
#include <stdio.h>

int gen_rsa(EVP_PKEY **pkey)
{
    // init ctx for key gen
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        fprintf(stderr, "1\n");
        return 1;
    }

    // setup keygen
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        fprintf(stderr, "2\n");
        EVP_PKEY_CTX_free(ctx);
        return 1;
    }
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        fprintf(stderr, "3\n");
        EVP_PKEY_CTX_free(ctx);
        return 1;
    }

    // gen key
    EVP_PKEY *_pkey = NULL;
    if (EVP_PKEY_keygen(ctx, &_pkey) <= 0) {
        fprintf(stderr, "4\n");
        EVP_PKEY_CTX_free(ctx);
        return 1;
    }

    EVP_PKEY_CTX_free(ctx);

    *pkey = _pkey;
    return 0;
}

uint8_t *get_rsa_der_pub(EVP_PKEY *pkey, int *len)
{
    uint8_t *rsa_key = NULL;
    int _len = i2d_PUBKEY(pkey, &rsa_key);
    if (_len <= 0)
        return NULL;
    *len = _len;
    return rsa_key;
}

uint8_t *rsa_encrypt(uint8_t *buf, const size_t buf_len,
                     uint8_t *rsa_der_pub_key, const size_t pub_key_len,
                     size_t *outlen)
{
    /* for VCMP peers, we should allocate a EVP_PKEY object and store it into
     * peer struct */
    const uint8_t *p = rsa_der_pub_key;
    EVP_PKEY *pkey = d2i_PUBKEY(NULL, &p, pub_key_len);
    if (!pkey) {
        fprintf(stderr, "failed loading public key from DER\n");
        return NULL;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx) {
        fprintf(stderr, "failed creating evp pkey ctx\n");
        EVP_PKEY_free(pkey);
        return NULL;
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        fprintf(stderr, "failed to init evp pkey ctx encryption\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    size_t _outlen = 0;
    if (EVP_PKEY_encrypt(ctx, NULL, &_outlen, buf, buf_len) <= 0) {
        fprintf(stderr, "failed to determine encrypted data len\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    uint8_t *encrypted = (uint8_t *)malloc(_outlen);
    if (!encrypted) {
        fprintf(stderr, "failed to alloc encrypted data buf\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_encrypt(ctx, encrypted, &_outlen, buf, buf_len) <= 0) {
        fprintf(stderr, "failed to encrypt data buf\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    *outlen = _outlen;
    return encrypted;
}

uint8_t *rsa_decrypt(EVP_PKEY *pkey, uint8_t *buf, const size_t buf_len,
                     size_t *outlen)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx) {
        fprintf(stderr, "failed creating evp pkey ctx\n");
        return NULL;
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        fprintf(stderr, "failed to init evp pkey ctx decryption\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    size_t _outlen = 0;
    if (EVP_PKEY_decrypt(ctx, NULL, &_outlen, buf, buf_len) <= 0) {
        fprintf(stderr, "failed to determine decrypted data len\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    uint8_t *decrypted = (uint8_t *)malloc(_outlen);
    if (!decrypted) {
        fprintf(stderr, "failed to alloc encrypted data buf\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_decrypt(ctx, decrypted, &_outlen, buf, buf_len) <= 0) {
        fprintf(stderr, "failed to encrypt data buf\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_CTX_free(ctx);

    *outlen = _outlen;
    return decrypted;
}

int main()
{
    EVP_PKEY *pkey = NULL;
    if (gen_rsa(&pkey) != 0) {
        fprintf(stderr, "failed to gen rsa keys\n");
        return 1;
    }

    fprintf(stdout, "generated rsa keys!\n");

    int pub_key_len = 0;
    uint8_t *pub_key = get_rsa_der_pub(pkey, &pub_key_len);
    if (!pub_key) {
        fprintf(stderr, "failed to get rsa pub key\n");
        EVP_PKEY_free(pkey);
        return 1;
    }

    fprintf(stdout, "rsa der pub key len: %d\n", pub_key_len);

    char data[] = "rsa test 123";

    size_t encrypted_len = 0;
    uint8_t *encrypted = rsa_encrypt((uint8_t *)data, sizeof(data) - 1, pub_key,
                                     pub_key_len, &encrypted_len);
    if (!encrypted) {
        fprintf(stderr, "failed to encrypt data\n");
        OPENSSL_free(pub_key);
        EVP_PKEY_free(pkey);
        return 1;
    }

    fprintf(stdout, "encrypted data len: %lu\n", encrypted_len);

    // decrypt
    size_t decrypted_len = 0;
    uint8_t *decrypted =
        rsa_decrypt(pkey, encrypted, encrypted_len, &decrypted_len);

    if (!decrypted) {
        fprintf(stderr, "failed to decrypt data\n");
        free(encrypted);
        OPENSSL_free(pub_key);
        EVP_PKEY_free(pkey);
        return 1;
    }

    fprintf(stdout, "decrypted data, len: %lu\n", decrypted_len);

    if (strncmp((const char *)decrypted, data, decrypted_len) != 0) {
        fprintf(stderr, "decrypted data doesn't match to original data\n");
        free(decrypted);
        free(encrypted);
        OPENSSL_free(pub_key);
        EVP_PKEY_free(pkey);
        return 1;
    }

    free(decrypted);
    free(encrypted);
    OPENSSL_free(pub_key);
    EVP_PKEY_free(pkey);
    return 0;
}