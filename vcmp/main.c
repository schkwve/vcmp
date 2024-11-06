#include "lib/ini.h"
#include "lib/log.h"
#include <event2/event.h>
#include <unistd.h>
#include <vcmp/api.h>
#include <vcmp/listener.h>

#define config_path "resources/config.ini"

int main()
{
    log_info("=== VCMP ===");

    ini_config *cfg = ini_parse(config_path);
    if (cfg == NULL) {
        goto error;
    }

    ini_dump_config(cfg);

    // get listener and API IP/Port
    char *listener_ip = ini_get_value(cfg, "listener", "address");
    char *listener_port_str = ini_get_value(cfg, "listener", "port");

    char *api_ip = ini_get_value(cfg, "wsapi", "address");
    char *api_port_str = ini_get_value(cfg, "wsapi", "port");

    ini_free(&cfg);

    // set errno to 0 so we can reliably detect if the string to int convertion
    // was successful
    errno = 0;

    uint16_t listener_port = (uint16_t)strtol(listener_port_str, NULL, 10);
    if (errno) {
        log_error("Listener port conversion error: %s", strerror(errno));
        goto error;
    }

    uint16_t api_port = (uint16_t)strtol(api_port_str, NULL, 10);
    if (errno) {
        log_error("Listener port conversion error: %s", strerror(errno));
        goto error;
    }

    // free all unneeded memory
    free(listener_port_str);
    free(api_port_str);
    listener_port_str = NULL;
    api_port_str = NULL;

    // Seed rand
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((unsigned int)(ts.tv_nsec ^ ts.tv_sec ^ getpid()));

    struct event_base *base = event_base_new();
    if (!base) {
        log_error("Failed to allocate a new event base.");
        goto error;
    }

    if (vcmp_listener_init(base, listener_ip, listener_port) != 0) {
        log_error("Failed to initialize VCMP listener.");
        goto error;
    }

    if (vcmp_api_init(base, api_ip, api_port) != 0) {
        log_error("Failed to initialize VCMP API.");
        goto error;
    }

    log_info("=== Initialized everything. Happy chatting! === ");

    event_base_dispatch(base);
    event_base_free(base);
    return 0;

error:
    if (base) {
        log_debug("Free 1");
        event_base_free(base);
    }

    if (cfg) {
        log_debug("Free 2");
        ini_free(&cfg);
    }
    if (listener_port_str) {
        log_debug("Free 3");
        free(listener_port_str);
    }
    if (api_port_str) {
        log_debug("Free 4");
        free(api_port_str);
    }
    return 1;
}
