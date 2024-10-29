#include "../lib/log.h"
#include <vcmp/api.h>
#include <websockets/frame.h>
#include <websockets/websockets.h>

static void read_cb(struct bufferevent *bev, const uint8_t opcode,
                    void *payload, const uint64_t len, void *ctx)
{
    switch (opcode) {
    case WS_OPCODE_CONTINUATION: {
        log_debug("Packet is continuing");
        break;
    }
    case WS_OPCODE_TEXT: {
        log_debug("Data received: %.*s", (int)len, (char *)payload);
        break;
    }
    case WS_OPCODE_BINARY: {
        log_debug("We received binary payload");
        break;
    }
    case WS_OPCODE_PING: {
        log_debug("Client has sent a PING");
        break;
    }
    case WS_OPCODE_PONG: {
        log_debug("Client has sent a PONG");
        break;
    }
    };

    char resp[] = "WebSockets!";
    int r = ws_client_send(bev, WS_OPCODE_TEXT, resp, sizeof(resp) - 1);
    if (r != 0) {
        log_warn("Failed to deliver a message to the client!\n");
    }
}

static void accept_cb(struct bufferevent *bev, struct ws_client *client)
{
    log_debug("Accepted a WebSocket client!");
}

static void close_cb(struct bufferevent *bev, void *ctx)
{
    log_debug("WebSocket client will close after this callback");
}

int vcmp_api_init(struct event_base *base, const char *ip, const uint16_t port)
{
    ws_ctx_t ctx;
    ctx.read_cb = read_cb;
    ctx.accept_cb = accept_cb;
    ctx.close_cb = close_cb;

    if (ws_server_init(base, ip, port, ctx) != 0) {
        log_error("Failed to init api (websocket) server");
        return 1;
    }

    return 0;
}