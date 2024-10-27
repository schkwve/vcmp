#include <stdlib.h>
#include <uuid/uuid.h>
#include <vcmp/util/uuid.h>

char *gen_uuid()
{
    char *uuid = malloc(UUID_STR_LEN);
    if (!uuid)
        return NULL;

    uuid_t binuuid;
    uuid_generate_random(binuuid);
    uuid_unparse_upper(binuuid, uuid);

    return uuid;
}