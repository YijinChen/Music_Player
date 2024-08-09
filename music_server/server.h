#ifndef SERVER_H
#define SERVER_H
#include <event2/event.h>
#include <json/json.h>

#define IP "18.185.92.160"
#define PORT 8000

class PlayerServer{
    private:
    struct event_base *base = NULL; //set for event
    struct evconnlistener *listener = NULL;

    //callback functions
    static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg); 
    static void read_cb(struct bufferevent *bev, void *ctx);
    static void event_cb(struct bufferevent *bev, short wait, void *ctx);

    public:
    PlayerServer(const char *ip = IP, int port = PORT);
    ~PlayerServer();

};


#endif