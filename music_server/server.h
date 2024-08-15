#ifndef SERVER_H
#define SERVER_H
#include <event2/event.h>
#include <json/json.h>
//#include <jsoncpp/json/json.h> //for ubuntu
#include <list>
#include <time.h>
#include "player.h"
#include "node.h"

#define IP "127.0.0.1"   //for macos
//#define IP "172.31.47.86"  //for ubuntu
#define PORT 8000

class PlayerServer{
    private:
    struct event_base *base; //set for event
    struct evconnlistener *listener;
    static std::list<Node> *l;
    static Player *p;

    //callback functions
    static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg); 
    static void read_cb(struct bufferevent *bev, void *ctx);
    static void event_cb(struct bufferevent *bev, short wait, void *ctx);

    public:
    PlayerServer(const char *ip = IP, int port = PORT);
    ~PlayerServer();

};


#endif