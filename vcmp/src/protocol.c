#include <vcmp/protocol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h>

#include "../lib/log.h"

void vcmp_gen_header(vcmp_header_t *header, const uint8_t type)
{
    memcpy(header->magic, VCMP_MAGIC, sizeof(VCMP_MAGIC));
    header->version = VCMP_VERSION;
    header->type = type;
}

vcmp_handshake_t *vcmp_gen_hs_header(const uint8_t type, const uint32_t keylen,
                                     const uint8_t *pubkey)
{
    vcmp_handshake_t *h =
        (vcmp_handshake_t *)malloc(sizeof(vcmp_handshake_t) + keylen);
    if (!h) {
        log_error("Failed to allocate vcmp handshake");
        return NULL;
    }
    vcmp_gen_header(&h->header, VCMP_TYPE_HS_INIT);
    h->keylen = keylen;
    memcpy(h->pubkey, pubkey, keylen);
    return h;
}

void vcmp_gen_data_header(vcmp_data_t *data, const uint32_t payload_size)
{
    vcmp_gen_header(&data->header, VCMP_TYPE_DATA);
    uuid_generate((uint8_t *)data->message_uuid);
    data->timestamp = (uint64_t)time(NULL);
    data->len = payload_size;
}