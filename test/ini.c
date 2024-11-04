#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum parser_state { READY_FOR_DATA, COMMENT, SECTION_NAME, KEY, VALUE };

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

/**
 * Creates a new section list.
 */
struct ini_section *new_section(const char *name)
{
    struct ini_section *new = malloc(sizeof(struct ini_section));
    if (new == NULL) {
        printf("Failed to allocate memory for new section.\n");
        return NULL;
    }

    new->name = strdup(name);
    new->keyvals = NULL;
    new->next = NULL;
    return new;
}

/**
 * Appends a new section to an existing list.
 */
struct ini_section *add_section(struct ini_section **root, const char *name)
{
    struct ini_section **cur_section;

    if (root == NULL) {
        printf("Invalid section root specified.\n");
        return NULL;
    }

    cur_section = root;
    while (*cur_section != NULL) {
        cur_section = &(*cur_section)->next;
    }

    *cur_section = new_section(name);
    return *cur_section;
}

/**
 * Creates a new key-value list.
 */
struct ini_keyval *new_keyval(const char *name, const char *value)
{
    struct ini_keyval *new = malloc(sizeof(struct ini_keyval));
    if (new == NULL) {
        printf("Failed to allocate memory for new key-value pair.\n");
        return NULL;
    }

    new->name = strdup(name);
    new->value = strdup(value);
    new->next = NULL;
    return new;
}

/**
 * Returns a pointer to the section. If the specified section does not exist,
 * create one.
 */
struct ini_section *find_or_create_section(ini_config *cfg, const char *name)
{
    if (cfg == NULL) {
        printf("Invalid configuration root specified.\n");
        return NULL;
    }

    if (name == NULL || name[0] == '\0') {
        printf("Invalid section name specified.\n");
        return NULL;
    }

    return add_section(&cfg->sections, name);
}

/**
 * Returns a pointer to the section or NULL.
 */
struct ini_section *find_section(ini_config *cfg, const char *name)
{
    struct ini_section *cur_section;

    if (cfg == NULL) {
        printf("Invalid configuration root specified.\n");
        return NULL;
    }

    if (name == NULL || name[0] == '\0') {
        printf("Invalid section name specified.\n");
        return NULL;
    }

    cur_section = cfg->sections;
    while (cur_section != NULL) {
        if (strcmp(cur_section->name, name) == 0) {
            return cur_section;
        }

        cur_section = cur_section->next;
    }

    return NULL;
}

/**
 * Appends a new key-value pair to an existing list.
 */
void add_keyval(struct ini_keyval **root, const char *name, const char *value)
{
    struct ini_keyval **cur_keyval;

    cur_keyval = root;
    while (*cur_keyval != NULL) {
        *cur_keyval = (*cur_keyval)->next;
    }

    *cur_keyval = new_keyval(name, value);
}

/**
 * Assigns a key-value pair to a section.
 */
void assign_keyval_to_section(struct ini_section *section, const char *name,
                              const char *value)
{
    if (section == NULL) {
        printf("Invalid section specified.\n");
        return;
    }

    if (name == NULL || name[0] == '\0') {
        printf("Invalid key name specified.\n");
        return;
    }

    add_keyval(&section->keyvals, name, value);
}

/**
 * Assigns a key-value pair to the config root (pairs without a section)
 */
void assign_keyval_to_root(struct ini_config *cfg, const char *name,
                           const char *value)
{
    if (cfg == NULL) {
        printf("Invalid configuration root specified.\n");
        return;
    }

    if (name == NULL || name[0] == '\0') {
        printf("Invalid key name specified.\n");
        return;
    }

    add_keyval(&cfg->global_keyvals, name, value);
}

/**
 * Assigns a key-value pair.
 * If section is NULL, the pair will be considered global.
 */
void assign_keyval(struct ini_config *cfg, const char *section,
                   const char *name, const char *value)
{
    if (cfg == NULL) {
        printf("Invalid configuration root specified.\n");
        return;
    }

    if (name == NULL || name[0] == '\0') {
        printf("Invalid key name specified.\n");
        return;
    }

    if (section == NULL) {
        // global key-value
        assign_keyval_to_root(cfg, name, value);
    }
    else {
        struct ini_section *sec = find_or_create_section(cfg, section);
        if (sec == NULL) {
            return;
        }
        assign_keyval_to_section(sec, name, value);
    }
}

// ***- PUBLIC -*** //

/**
 * Create a new configuration structure.
 */
ini_config *ini_create(void)
{
    ini_config *new = malloc(sizeof(struct ini_config));
    if (new == NULL) {
        printf("Failed to allocate memory for configuration structure.\n");
        return NULL;
    }

    new->sections = NULL;
    new->global_keyvals = NULL;
    return new;
}

/**
 * Free the configuration structure.
 */
void ini_free(ini_config *cfg)
{
    struct ini_section *cur_section, *next_section;
    struct ini_keyval *cur_keyval, *next_keyval;

    if (!cfg)
        return;

    cur_keyval = cfg->global_keyvals;
    while (cur_keyval != NULL) {
        next_keyval = cur_keyval->next;
        free(cur_keyval);
        cur_keyval = next_keyval;
    }

    cur_section = cfg->sections;
    if (cur_section == NULL) {
        goto final;
    }

    while (cur_section != NULL) {
        next_section = cur_section->next;

        cur_keyval = cur_section->keyvals;
        while (cur_keyval != NULL) {
            next_keyval = cur_keyval->next;
            free(cur_keyval->name);
            free(cur_keyval->value);
            free(cur_keyval);
            cur_keyval = next_keyval;
        }

        free(cur_section->name);
        free(cur_section);

        cur_section = next_section;
    }
final:
    free(cfg);
}

/**
 * Get a value from a key.
 * If the section is NULL, the key will be considered global.
 * If the key (or the specified section) was not found,
 *   the function will return NULL.
 *
 * Keys and section names are case-sensitive.
 */
char *ini_get_value(ini_config *cfg, const char *section, const char *key)
{
    struct ini_keyval *cur_keyval;

    if (cfg == NULL) {
        printf("Supplied invalid configuration structure.\n");
        return NULL;
    }

    if (key == NULL || key[0] == '\0') {
        printf("Supplied invalid key name.\n");
        return NULL;
    }

    if (section == NULL) {
        cur_keyval = cfg->global_keyvals;
    }
    else {
        struct ini_section *sec = find_section(cfg, section);
        if (sec == NULL) {
            printf("Specified section '%s' was not found.\n", section);
            return NULL;
        }
        cur_keyval = sec->keyvals;
    }

    while (cur_keyval != NULL) {
        if (strcmp(cur_keyval->name, key) == 0) {
            return cur_keyval->value;
        }

        cur_keyval = cur_keyval->next;
    }

    return NULL;
}

/**
 * Parse a configuration file from a supplied buffer.
 *
 * This function does not free the buffer memory.
 */
ini_config *ini_parsebuf(const char *buf)
{
    int state = READY_FOR_DATA;
    const char *bufp = buf;

    // buffer for temporary use by the parser
    char tmp[256] = {0};

    char current_section[256] = {0};
    char current_key[256] = {0};
    char current_value[256] = {0};

    ini_config *cfg = NULL;

    // This function will probably never be called from outside of this library/
    // but it's better to do some checks to be safe.
    if (buf == NULL || buf[0] == '\0') {
        printf("Invalid buffer.\n");
        return NULL;
    }

    cfg = ini_create();
    if (cfg == NULL) {
        return NULL;
    }

    while (*bufp != '\0') {
        switch (state) {
        case READY_FOR_DATA: {
            if (*bufp == '#' || *bufp == ';') {
                state = COMMENT;
            }
            else if (*bufp == '[') {
                state = SECTION_NAME;
            }
            else if (*bufp != '\n' && *bufp != ' ') {
                state = KEY;
            }

            break;
        }
        case COMMENT: {
            while (*bufp != '\r' && *bufp != '\n' && *bufp != '\t' &&
                   *bufp != '\0') {
                bufp++;
            }

            state = READY_FOR_DATA;
            break;
        }
        case SECTION_NAME: {
            int i = 0;
            while (*bufp != ']' && i != 255) {
                if (*bufp == '\0' || *bufp == '\n') {
                    printf("Invalid section.\n");
                    state = READY_FOR_DATA;

                    goto fail;
                }
                tmp[i++] = *bufp++;
            }

            memcpy(current_section, tmp, 256 * sizeof(char));
            memset(&tmp, 0, 256);

            state = READY_FOR_DATA;
            break;
        }
        case KEY: {
            int i = 0;
            bufp--; // take last cycle back
            while (*bufp != ' ' && *bufp != '\t' && *bufp != '=' && i != 256) {
                if (*bufp == '\0' || *bufp == '\n') {
                    printf("Invalid key-value pair.\n");
                    state = READY_FOR_DATA;

                    goto fail;
                }
                tmp[i++] = *bufp++;
            }

            while (*bufp != '=') {
                if (*bufp == '\0' || *bufp == '\n') {
                    printf("Invalid key-value pair.\n");
                    state = READY_FOR_DATA;

                    goto fail;
                }
                bufp++;
            }

            memcpy(current_key, tmp, 256 * sizeof(char));
            memset(&tmp, 0, 256);

            state = VALUE;
            break;
        }
        case VALUE: {
            int i = 0;

            while (*bufp == ' ' || *bufp == '\t') {
                bufp++;
            }

            while (*bufp != '\n' && *bufp != '\0') {
                tmp[i++] = *bufp++;
            }

            memcpy(current_value, tmp, 256 * sizeof(char));
            memset(&tmp, 0, 256);

            assign_keyval(cfg,
                          (current_section[0] == '\0') ? NULL : current_section,
                          current_key, current_value);

            state = READY_FOR_DATA;
            break;
        }
        default: {
            printf("State machine broke.\n");

            goto fail;
            break;
        }
        }

        bufp++;
    }

    return cfg;

fail:
    if (cfg) {
        ini_free(cfg);
    }
    return NULL;
}

int main(void)
{
    char *test_ini = "# This is a test of an INI parser\n"
                     "; (comments are prefixed with either # or ;)\n"
                     "global=variable123\n"
                     "[section1]\n"
                     "coolvar=one\n"
                     "[section2]\n"
                     "coolvar=two\n"
                     "test=test\n"
                     "[section3]\n";

    ini_config *cfg = ini_parsebuf(test_ini);
    char *tmp;

    printf("Test 1: ini_get_value() --");
    fflush(stdout);
    tmp = ini_get_value(cfg, "section1", "coolvar");
    if (strcmp(tmp, "one") != 0) {
        printf(" FAILED!\nExpected: one; Got: %s\n", tmp);
    }
    else {
        puts(" PASSED");
    }

    printf("Test 2: ini_get_value() --");
    fflush(stdout);
    tmp = ini_get_value(cfg, "section2", "coolvar");
    if (tmp == NULL) {
        printf(" FAILED!\nExpected: two; Got: (not found)\n");
    }
    else if (strcmp(tmp, "two") != 0) {
        printf(" FAILED!\nExpected: two; Got: %s\n", tmp);
    }
    else {
        printf(" PASSED\n");
    }

    printf("All tests done! Freeing allocated memory...\n");
    ini_free(cfg);

    return 0;
}
