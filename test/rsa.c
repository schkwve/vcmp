#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <stdio.h>

struct rsa_keys {
    BIO *priv;
    BIO *pub;
};

int gen_rsa(struct rsa_keys *keys)
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
    EVP_PKEY *pkey = NULL;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        fprintf(stderr, "4\n");
        EVP_PKEY_CTX_free(ctx);
        return 1;
    }

    // allocate BIOs
    BIO *priv_bio = BIO_new(BIO_s_mem());
    BIO *pub_bio = BIO_new(BIO_s_mem());
    if (!priv_bio || !pub_bio) {
        fprintf(stderr, "5\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return 1;
    }

    // encode in DER
    if (i2d_PrivateKey_bio(priv_bio, pkey) <= 0) {
        fprintf(stderr, "6\n");
        BIO_free(priv_bio);
        BIO_free(pub_bio);
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return 1;
    }

    if (i2d_PUBKEY_bio(pub_bio, pkey) <= 0) {
        fprintf(stderr, "7\n");
        BIO_free(priv_bio);
        BIO_free(pub_bio);
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return 1;
    }

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    keys->priv = priv_bio;
    keys->pub = pub_bio;

    return 0;
}

int main()
{
    struct rsa_keys keys = {0};
    if (gen_rsa(&keys) != 0) {
        printf("failed to gen rsa keys\n");
        return 1;
    }

    printf("generated rsa keys!\n");

    // free keys
    BIO_free(keys.priv);
    BIO_free(keys.pub);
    return 0;
}