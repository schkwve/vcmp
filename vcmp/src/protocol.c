#include <vcmp/protocol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h>

#include "../lib/log.h"

int vcmp_gen_header(uint8_t *buf, const uint32_t buf_len)
{
    if (buf_len < sizeof(vcmp_header_t)) {
        log_error("Cannot fit VCMP header into buf!");
        return 1;
    }

    vcmp_header_t *header = (vcmp_header_t *)buf;

    header->magic[0] = 'V';
    header->magic[1] = 'C';
    header->magic[2] = 'M';
    header->magic[3] = 'P';
    header->magic[4] = '\0';

    header->version = VCMP_VERSION;
    uuid_generate((uint8_t *)header->message_uuid);

    time_t current_time = time(NULL);
    if (current_time == (time_t)-1) {
        log_error("Timestamp is NULL!");
        return 1;
    }

    header->timestamp = (uint64_t)current_time;

    /* Fill out and think of a nice interface to fill next 2 header members */

    return 0;
}