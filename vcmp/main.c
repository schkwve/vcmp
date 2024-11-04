#include "lib/log.h"
#include <event2/event.h>
#include <unistd.h>
#include <vcmp/api.h>
#include <vcmp/listener.h>

int main()
{
    log_info("=== VCMP ===");

    // Seed rand
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((unsigned int)(ts.tv_nsec ^ ts.tv_sec ^ getpid()));

    struct event_base *base = event_base_new();
    if (!base) {
        log_error("Failed to allocate a new event base.");
        return 1;
    }

    if (vcmp_listener_init(base, "127.0.0.1", 55555) != 0) {
        log_error("Failed to initialize VCMP listener.");
        event_base_free(base);
        return 1;
    }

    if (vcmp_api_init(base, "127.0.0.1", 44444) != 0) {
        log_error("Failed to initialize VCMP API.");
        event_base_free(base);
        return 1;
    }

    log_info("=== Initialized everything. Happy chatting! === ");

    event_base_dispatch(base);
    event_base_free(base);
    return 0;
}
