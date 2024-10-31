#ifndef _VCMP_LISTENER_H
#define _VCMP_LISTENER_H

#include <event2/event.h>
#include <stdint.h>

typedef struct vcmp_peer {

} vcmp_peer_t;

int vcmp_listener_init(struct event_base *base, const char *ip,
                       const uint16_t port);

#endif // _VCMP_LISTENER_H