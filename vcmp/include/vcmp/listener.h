#ifndef _VCMP_LISTENER_H
#define _VCMP_LISTENER_H

#include "rsa.h"
#include <event2/event.h>
#include <stdint.h>

#define VCMP_PEER_STAGE_NONE 0
#define VCMP_PEER_STAGE_PUBKEY_WAIT 1
#define VCMP_PEER_STAGE_PUBKEY_CHECK 2
#define VCMP_PEER_STAGE_READY 3

typedef struct vcmp_peer {
    EVP_PKEY *pubkey;
    int stage;
    char authenticity[12];
} vcmp_peer_t;

int vcmp_listener_init(struct event_base *base, const char *ip,
                       const uint16_t port);

#endif // _VCMP_LISTENER_H