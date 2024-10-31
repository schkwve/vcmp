#include "../lib/log.h"
#include <vcmp/rsa.h>

EVP_PKEY *rsa_generate()
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        log_error("Failed to alloc a evp pkey ctx\n");
        return NULL;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        log_error("Failed to init evp pkey keygen\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, RSA_BITS) <= 0) {
        log_error("Failed to set rsa bits to evp pkey ctx\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY *pkey = NULL;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        log_error("Failed to generate keys\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

uint8_t *rsa_get_der_pubkey(EVP_PKEY *pubkey, int *outlen)
{
    uint8_t *key = NULL;
    *outlen = i2d_PUBKEY(pubkey, &key);
    if (*outlen <= 0) {
        log_error("Failed to convert key to DER\n");
        return NULL;
    }
    return key;
}

EVP_PKEY *rsa_load_der_pubkey(const uint8_t *pubkey, const size_t keylen)
{
    const uint8_t *p = pubkey;
    EVP_PKEY *pkey = d2i_PUBKEY(NULL, &p, keylen);
    if (!pkey) {
        log_error("Failed loading public key from DER\n");
        return NULL;
    }
    return pkey;
}

uint8_t *rsa_encrypt(const uint8_t *buf, const size_t buflen, EVP_PKEY *pubkey,
                     size_t *outlen)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pubkey, NULL);
    if (!ctx) {
        log_error("Failed to init evp pkey keygen\n");
        return NULL;
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        log_error("Failed to init evp pkey ctx encryption\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_encrypt(ctx, NULL, outlen, buf, buflen) <= 0) {
        log_error("Failed to determine encrypted data len\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    uint8_t *encrypted = (uint8_t *)malloc(*outlen);
    if (!encrypted) {
        log_error("Failed to alloc encrypted data buf\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_encrypt(ctx, encrypted, outlen, buf, buflen) <= 0) {
        log_error("Failed to encrypt data buf\n");
        free(encrypted);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_CTX_free(ctx);
    return encrypted;
}

uint8_t *rsa_decrypt(const uint8_t *buf, const size_t buflen, EVP_PKEY *privkey,
                     size_t *outlen)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(privkey, NULL);
    if (!ctx) {
        log_error("Failed to init evp pkey keygen\n");
        return NULL;
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        log_error("Failed to init evp pkey ctx decryption\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_decrypt(ctx, NULL, outlen, buf, buflen) <= 0) {
        log_error("Failed to determine decrypted data len\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    uint8_t *decrypted = (uint8_t *)malloc(*outlen);
    if (!decrypted) {
        log_error("Failed to alloc decrypted data buf\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_encrypt(ctx, decrypted, outlen, buf, buflen) <= 0) {
        log_error("Failed to decrypt data buf\n");
        free(decrypted);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_CTX_free(ctx);
    return decrypted;
}