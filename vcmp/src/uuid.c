#include <vcmp/util/uuid.h>

char *gen_uuid() {
    uuid_t binuuid;

    uuid_generate_random(binuuid);
    char* uuid = malloc(UUID_STR_LEN);
    uuid_unparse_upper(binuuid, uuid);

    return uuid;
}