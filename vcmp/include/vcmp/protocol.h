#ifndef _VCMP_PROTOCOL_H
#define _VCMP_PROTOCOL_H

#include <stdint.h>

static const uint8_t VCMP_MAGIC[5] = {'V', 'C', 'M', 'P', '\0'};
#define VCMP_VERSION 1

// Packet IDS

#define VCMP_TYPE_VERIFY 1

#define VCMP_TYPE_HS_INIT 2
#define VCMP_TYPE_HS_ACK 3

#define VCMP_TYPE_DATA 4

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
                                     const uint8_t *pubkey);
void vcmp_gen_data_header(vcmp_data_t *data, const uint32_t payload_size);

#endif // _VCMP_PROTOCOL_H