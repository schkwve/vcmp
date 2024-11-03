#ifndef _VCMP_SOCKS5_H
#define _VCMP_SOCKS5_H

#include <event2/event.h>
#include <netinet/in.h>
#include <stdint.h>

#define SOCKS5_VERSION 0x05
#define SOCKS5_RESERVED 0x00
#define SOCKS5_SUBNEGOTIATION_VERSION 0x01

// Address types

#define SOCKS5_ATYP_IPV4 0x01
#define SOCKS5_ATYP_DOMAINNAME 0x03
#define SOCKS5_ATYP_IPV6 0x04

// Command codes

#define SOCKS5_CMD_CONNECT 0x01
#define SOCKS5_CMD_BIND 0x02
#define SOCKS5_CMD_UDP_ASSOCIATE 0x03

// Reply field values

#define SOCKS5_REPLY_SUCCEEDED 0x00
#define SOCKS5_REPLY_GENERAL_FAILURE 0x01
#define SOCKS5_REPLY_CONNECTION_NOT_ALLOWED 0x02
#define SOCKS5_REPLY_NETWORK_UNREACHABLE 0x03
#define SOCKS5_REPLY_HOST_UNREACHABLE 0x04
#define SOCKS5_REPLY_CONNECTION_REFUSED 0x05
#define SOCKS5_REPLY_TTL_EXPIRED 0x06
#define SOCKS5_REPLY_COMMAND_NOT_SUPPORTED 0x07
#define SOCKS5_REPLY_ADDRESS_TYPE_NOT_SUPPORTED 0x08

// Authentication methods

#define SOCKS5_METHOD_NONE 0x00
#define SOCKS5_METHOD_GSSAPI 0x01
#define SOCKS5_METHOD_USERNAME_PASSWORD 0x02
#define SOCKS5_METHOD_IANA_ASSIGNED_START 0x03
#define SOCKS5_METHOD_IANA_ASSIGNED_END 0x7F
#define SOCKS5_METHOD_RESERVED_START 0x80
#define SOCKS5_METHOD_RESERVED_END 0xFE
#define SOCKS5_METHOD_NO_ACCEPTABLE 0xFF

// Stages

#define SOCKS5_STAGE_IDENT 1
#define SOCKS5_STAGE_AUTH 2
#define SOCKS5_STAGE_REQUEST 3
#define SOCKS5_STAGE_CONNECTED 4

typedef void (*socks5_read_cb)(struct bufferevent *bev, void *ctx);
typedef void (*socks5_write_cb)(struct bufferevent *bev, void *ctx);
typedef void (*socks5_event_cb)(struct bufferevent *bev, short events,
                                void *ctx);

typedef struct socks5_hostname {
    int len;
    char hostname[256];
} socks5_hostname_t;

typedef struct socks5_ctx {
    socks5_hostname_t *hostname;
    unsigned int *ipv4_addr;
    uint16_t dst_port;

    socks5_read_cb readcb;
    socks5_write_cb writecb;
    socks5_event_cb eventcb;
    void *ctx;

    unsigned int stage;
} socks5_ctx_t;

socks5_ctx_t *socks5_ctx_new_hostname(const char *dst_hostname,
                                      const uint16_t dst_port,
                                      socks5_read_cb readcb,
                                      socks5_write_cb writecb,
                                      socks5_event_cb eventcb, void *ctx);
socks5_ctx_t *socks5_ctx_new_ipv4(const char *dst_addr, const uint16_t dst_port,
                                  socks5_read_cb readcb,
                                  socks5_write_cb writecb,
                                  socks5_event_cb eventcb, void *ctx);
void socks5_ctx_free(socks5_ctx_t *ctx);

int socks5_client_init(struct event_base *base, const char *proxy_ip,
                       const uint16_t proxy_port, socks5_ctx_t *ctx);

#endif // _VCMP_SOCKS5_H