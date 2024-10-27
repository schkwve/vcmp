#include <include/vcmp/protocol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h>

int vcmp_gen_header(uint8_t *buf, const uint8_t buf_len)
{

    vcmp_header_t header = {0};

    uint8_t magic[5] = {'V', 'C', 'M', 'P', '\0'};
    memcpy(header.magic, magic, sizeof(header.magic));

    header.version = 1; // version 1.0

    uuid_t uuid;
    uuid_generate(uuid);
    memcpy(&header.message_uuid[0], uuid, sizeof(uint64_t));
    memcpy(&header.message_uuid[1], uuid + sizeof(uint64_t), sizeof(uint64_t));

    header.timestamp = 0;
    time_t current_time = time(NULL);
    if (current_time == (time_t)-1) {
        perror("Timestamp is NULL!");
        return 1;
    }
    header.timestamp = (uint64_t)current_time;

    uint8_t buffer[sizeof(header)];

    return 0;
}