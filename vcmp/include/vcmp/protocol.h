#ifndef _VCMP_PROTOCOL_H
#define _VCMP_PROTOCOL_H

#include <stdint.h>

#define VCMP_VERSION 1

// Handshake types

#define VCMP_TYPE_HS_INIT 1
#define VCMP_TYPE_HS_ACK 2

#define VCMP_TYPE_DATA 3

typedef struct vcmp_header {
    uint8_t magic[5];
    uint8_t version;
    uint8_t type;
} __attribute__((packed)) vcmp_header_t;

typedef struct vcmp_handshake {
    vcmp_header_t header;
    uint32_t keylen;
    uint8_t pubkey[];
} __attribute__((packed)) vcmp_handshake_t;

typedef struct vcmp_data {
    vcmp_header_t header;
    uint64_t message_uuid[2];
    uint64_t timestamp;
    uint32_t len;
    uint8_t payload[];
} __attribute__((packed)) vcmp_data_t;

void vcmp_gen_header(vcmp_header_t *header, const uint8_t type);
vcmp_handshake_t *vcmp_gen_hs_header(const uint8_t type, const uint32_t keylen,
                                     const uint8_t *pubkey, int *outlen);
void vcmp_gen_data_header(vcmp_data_t *data, const uint32_t payload_size);

#endif // _VCMP_PROTOCOL_H