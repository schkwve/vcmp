#include <event2/bufferevent.h>
#include <event2/event.h>
#include <stdio.h>
#include <vcmp/socks5.h>

static int connected = 0;

static void eventcb(struct bufferevent *bev, short events, void *ctx)
{
    struct event_base *base = (struct event_base *)ctx;
    if (events & BEV_EVENT_CONNECTED) {
        fprintf(stdout, "connected to google!\n");
        connected = 1;
    }
    event_base_loopexit(base, NULL);
}

int main()
{
    struct event_base *base = event_base_new();

    socks5_ctx_t *ctx =
        socks5_ctx_new_hostname("google.com", 443, NULL, NULL, eventcb, base);

    // Passing TOR proxy
    if (socks5_client_init(base, "127.0.0.1", 9050, ctx) != 0) {
        event_base_free(base);
        fprintf(stderr, "failed to init socks5 client\n");
        return 1;
    }

    event_base_dispatch(base);
    event_base_free(base);

    if (!connected) {
        fprintf(stderr, "failed to connect to dst\n");
        return 1;
    }

    return 0;
}