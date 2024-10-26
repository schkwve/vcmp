#include <vcmp/listener.h>
#include <netinet/in.h>

#include <event2/util.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

#include <string.h>
#include "../lib/log.h"

static void read_cb(struct bufferevent *bev, void *ctx)
{
    log_info("Got some data from peer");
}

static void write_cb(struct bufferevent *bev, void *ctx)
{

}

static void event_cb(struct bufferevent *bev, short events, void *ctx)
{

    log_info("Got some event from peer");
}

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *address, int socklen, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);

    if (!bev) {
        log_warn("Failed to allocate a new bufferevent for peer");
        return;
    }

    bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    log_error("Got an error %d (%s) on the listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));

    event_base_loopexit(base, NULL);
}

int vcmp_listener_init(struct event_base *base, const char *ip,
    const uint16_t port)
{
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if (evutil_inet_pton(AF_INET, ip, &sin.sin_addr) != 1) {
        log_error("Failed to convert address to network order");
        return 1;
    }

    struct evconnlistener *listener = evconnlistener_new_bind(base,
        accept_conn_cb, NULL, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
        (struct sockaddr *)&sin, sizeof(sin));

    if (!listener) {
        log_error("Couldn't create listener");
        return 1;
    }

    evconnlistener_set_error_cb(listener, accept_error_cb);
    return 0;
}