#include "../lib/log.h"
#include "../lib/yyjson.h"
#include <event2/bufferevent.h>
#include <vcmp/api.h>
#include <websockets/frame.h>
#include <websockets/websockets.h>

static struct bufferevent *api_user_bev = NULL;

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

        yyjson_doc *doc = yyjson_read(payload, len, 0);
        yyjson_val *root = yyjson_doc_get_root(doc);

        yyjson_doc_free(doc);
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
        log_warn("Failed to deliver a message to the client!");
    }
}

static void accept_cb(struct bufferevent *bev, struct ws_client *client)
{
    if (api_user_bev != NULL) {
        log_debug("There is already an user on api!");
        return;
    }
    log_debug("Accepted a WebSocket client!");
    api_user_bev = bev;

    // Testing
    log_info("%d", vcmp_api_event_user(VCMP_EVENT_USER_JOIN, "test"));
    log_info("%d", vcmp_api_event_user(VCMP_EVENT_USER_JOIN, "test2"));
}

static void close_cb(struct bufferevent *bev, void *ctx)
{
    log_debug("WebSocket client will close after this callback");
    api_user_bev = NULL;
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

int vcmp_api_event_user(const uint8_t type, const char *username)
{
    if (!api_user_bev) {
        log_warn("There is no user behind API");
        return 1;
    }

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    const char *t = type == VCMP_EVENT_USER_JOIN ? "user_join" : "user_leave";
    yyjson_mut_obj_add_str(doc, root, "event", t);
    yyjson_mut_obj_add_str(doc, root, "username", username);

    size_t len = 0;
    const char *json = yyjson_mut_write(doc, 0, &len);
    if (!json) {
        log_error("Failed to make a json");
        return 1;
    }

    int ret = ws_client_send(api_user_bev, WS_OPCODE_TEXT, json, len);
    free((void *)json);
    yyjson_mut_doc_free(doc);

    if (ret != 0) {
        log_error("Failed to send json to api user websocket");
        return 1;
    }

    return 0;
}