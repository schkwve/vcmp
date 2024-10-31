#ifndef _VCMP_RSA_H
#define _VCMP_RSA_H

#include <openssl/x509.h>
#include <stdlib.h>

#define RSA_BITS 2048

// Returns a pair of keys: private and public
EVP_PKEY *rsa_generate();

uint8_t *rsa_get_der_pubkey(EVP_PKEY *pubkey, int *outlen);
EVP_PKEY *rsa_load_der_pubkey(const uint8_t *pubkey, const size_t keylen);

uint8_t *rsa_encrypt(const uint8_t *buf, const size_t buflen, EVP_PKEY *pubkey,
                     size_t *outlen);
uint8_t *rsa_decrypt(const uint8_t *buf, const size_t buflen, EVP_PKEY *privkey,
                     size_t *outlen);

#endif // _VCMP_RSA_H