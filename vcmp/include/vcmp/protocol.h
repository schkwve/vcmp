#ifndef _VCMP_PROTOCOL_H
#define _VCMP_PROTOCOL_H

#include <stdint.h>

#define VCMP_VERSION 1

typedef struct vcmp_header {
    uint8_t magic[5];
    uint8_t version;
    uint64_t message_uuid[2];
    uint64_t timestamp;
    uint32_t payload_size;
} __attribute__((packed)) vcmp_header_t;

int vcmp_gen_header(uint8_t *buf, const uint32_t buf_len,
                    const uint32_t payload_size);

#endif // _VCMP_PROTOCOL_H