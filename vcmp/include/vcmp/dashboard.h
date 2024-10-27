#ifndef _VCMP_DASHBOARD_H
#define _VCMP_DASHBOARD_H

#include <event2/event.h>
#include <stdint.h>

int vcmp_dashboard_init(struct event_base *base, const char *ip,
    const uint16_t port);

#endif // _VCMP_DASHBOARD_H