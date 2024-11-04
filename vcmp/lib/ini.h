#ifndef _LIB_INI_H
#define _LIB_INI_H

struct ini_keyval {
    char *name;
    char *value;

    struct ini_keyval *next;
};

struct ini_section {
    char *name;
    struct ini_keyval *keyvals;
    struct ini_section *next;
};

typedef struct ini_config {
    struct ini_section *sections;
    struct ini_keyval *global_keyvals;
} ini_config;

ini_config *ini_create(void);
void ini_free(ini_config *cfg);

char *ini_get_value(ini_config *cfg, const char *section, const char *key);

ini_config *ini_parse(const char *filepath);
ini_config *ini_parsebuf(const char *buf);

// #ifdef _DEBUG
#include "log.h"
#include <stddef.h>

static void ini_dump_config(ini_config *cfg)
{
    struct ini_section *cur_section;
    struct ini_keyval *cur_keyval;

    if (cfg == NULL) {
        return;
    }

    log_debug("Dumping configuration:");
    log_debug("======================");

    cur_keyval = cfg->global_keyvals;
    if (cur_keyval != NULL) {
        log_debug("(Global)");
        while (cur_keyval != NULL) {
            log_debug("%s = %s", cur_keyval->name, cur_keyval->value);
            cur_keyval = cur_keyval->next;
        }
    }

    cur_section = cfg->sections;
    while (cur_section != NULL) {
        log_debug("[%s]", cur_section->name);

        cur_keyval = cur_section->keyvals;
        while (cur_keyval != NULL) {
            log_debug("%s = %s", cur_keyval->name, cur_keyval->value);
            cur_keyval = cur_keyval->next;
        }
        cur_section = cur_section->next;
    }
}
// #else
// #define ini_dump_config(...)
// #endif // _DEBUG

#endif // _LIB_INI_H
