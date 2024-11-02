#ifndef _VCMP_API_H
#define _VCMP_API_H

#include <event2/event.h>
#include <stdint.h>

#define VCMP_EVENT_USER_JOIN 1
#define VCMP_EVENT_USER_LEAVE 2

/* Initializes a websocket API */
int vcmp_api_init(struct event_base *base, const char *ip, const uint16_t port);

int vcmp_api_event_user(const uint8_t type, const char *username);

#endif // _VCMP_API_H