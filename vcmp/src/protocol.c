#include <vcmp/protocol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h>

#include "../lib/log.h"

int vcmp_gen_data_header(uint8_t *buf, const uint32_t buf_len,
                         const uint32_t payload_size)
{
    if (buf_len < sizeof(vcmp_data_header_t)) {
        log_error("Cannot fit VCMP header into buf!");
        return 1;
    }

    vcmp_data_header_t *header = (vcmp_data_header_t *)buf;

    header->magic[0] = 'V';
    header->magic[1] = 'C';
    header->magic[2] = 'M';
    header->magic[3] = 'P';
    header->magic[4] = '\0';

    header->version = VCMP_VERSION;
    uuid_generate((uint8_t *)header->message_uuid);

    header->timestamp = (uint64_t)time(NULL);
    header->payload_size = payload_size;

    return 0;
}