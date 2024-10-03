// Ubuntu Server (server_libevent.c)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8000
#define SERVER_IP "172.31.47.86"  // Replace with your server's IP

void event_cb(struct bufferevent *bev, short events, void *arg) {
    if (events & BEV_EVENT_ERROR) {
        perror("Error from bufferevent");
    }
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        bufferevent_free(bev);
    }
}

void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx) {
    struct event_base *base = (struct event_base *)ctx;
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, NULL, NULL, event_cb, NULL);
    bufferevent_enable(bev, EV_WRITE);

    char *message = "Hello from Ubuntu with libevent!";
    bufferevent_write(bev, message, strlen(message));
}

int main() {
    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in sin;

    base = event_base_new();
    if (!base) {
        perror("Couldn't create event base");
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SERVER_PORT);
    sin.sin_addr.s_addr = inet_addr(SERVER_IP);

    listener = evconnlistener_new_bind(base, accept_conn_cb, base, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, 
                                        (struct sockaddr *)&sin, sizeof(sin));

    if (!listener) {
        perror("Couldn't create listener");
        return 1;
    }

    printf("Server listening on %s:%d\n", SERVER_IP, SERVER_PORT);

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}
