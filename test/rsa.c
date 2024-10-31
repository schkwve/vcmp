#include <openssl/bio.h>
#include <openssl/crypto.h>
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

int main()
{
    EVP_PKEY *pkey = NULL;
    if (gen_rsa(&pkey) != 0) {
        printf("failed to gen rsa keys\n");
        return 1;
    }

    printf("generated rsa keys!\n");

    int pub_key_len = 0;
    uint8_t *pub_key = get_rsa_der_pub(pkey, &pub_key_len);
    if (!pub_key) {
        printf("failed to get rsa pub key\n");
        EVP_PKEY_free(pkey);
        return 1;
    }

    printf("rsa der pub key len: %d\n", pub_key_len);

    OPENSSL_free(pub_key);
    EVP_PKEY_free(pkey);
    return 0;
}