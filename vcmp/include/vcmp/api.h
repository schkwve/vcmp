#ifndef _VCMP_API_H
#define _VCMP_API_H

#include <event2/event.h>
#include <stdint.h>

/* Initializes a websocket API */
int vcmp_api_init(struct event_base *base, const char *ip, const uint16_t port);

#endif // _VCMP_API_H