#ifndef PLAYER_H
#define PLAYER_H
#include <list>
#include "node.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
//#include <json/json.h>
#include <jsoncpp/json/json.h>  //for ubuntu
#include <string.h>
#include <time.h>
#include <iostream>
#include <event2/util.h>

#define TIMEOUT 1  //keep the same as TIMEOUT in music_player/sokect.h

class Player{
    private:

    public:
    void player_alive_info(std::list<Node> *l, struct bufferevent *bev, Json::Value val, struct event_base *base);
    void player_operation(std::list<Node> *l, struct bufferevent *app_bev, const char* cmd);
    void player_reply_result(std::list<Node> *l, struct bufferevent *bev, Json::Value val);

    static void timeout_cb(evutil_socket_t fd, short event, void *arg);
};

void send_to_device(struct bufferevent *bev, std::string str);

#endif