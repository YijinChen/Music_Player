#ifndef PLAYER_H
#define PLAYER_H
#include <list>
#include "node.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <json/json.h>
#include <string.h>
#include <time.h>
#include <iostream>

class Player{
    private:

    public:
    void player_alive_info(std::list<Node> *l, struct bufferevent *bev, Json::Value val);
    void player_operation(std::list<Node> *l, struct bufferevent *app_bev, const char* cmd);
};

#endif