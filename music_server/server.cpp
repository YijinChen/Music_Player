#include "server.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

//static class variable should be initilized outside the class
std::list<Node> *PlayerServer::l = new std::list<Node>();
Player *PlayerServer::p = new Player(); 

PlayerServer::PlayerServer(const char *ip, int port){
    base = event_base_new();     //create set for events

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    listener = evconnlistener_new_bind(base, listener_cb, base, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(listener == NULL){
        std::cout << "evconnlistener_new_bind error\n" << strerror(errno) << std::endl;
    }
    event_base_dispatch(base);    //listen to the set
}

PlayerServer::~PlayerServer(){
    //free two objects
    evconnlistener_free(listener);
    event_base_free(base);
}

void PlayerServer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg){
    struct event_base *base = (struct event_base *)arg; 
    std::cout << "Client is applying for connect " << fd << std::endl;

    //create buffer event
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(bev == NULL){
        std::cout << "bufferevent_socket_new error\n";
    }
    //here send base to read_cb, so that read_cb (static function) can use base (unstatic member variable)
    bufferevent_setcb(bev, read_cb, NULL, event_cb, base);
    bufferevent_enable(bev, EV_READ);
}

void PlayerServer::read_cb(struct bufferevent *bev, void *ctx){
    struct event_base *base = (struct event_base *)ctx;
    
    char buf[1024] = {0};
    size_t ret = bufferevent_read(bev, buf, sizeof(buf));
    if(ret < 0){
        std::cout << "bufferevent_read error" << std::endl;
    }

    Json::Reader reader; //analysis json object
    Json::Value val; //analyzed result
    bool result = reader.parse(buf,val);
    if(!result){
        std::cout << "fail to parse json" << std::endl;
        return;
    }

    char cmd[32] = {0};
    strcpy(cmd, val["cmd"].asString().c_str());
    //parse message from app
    if(!strcmp(cmd, "bind")){ //received "bind" from app
        Node n;
        n.app_bev = bev;
        strcpy(n.app_id, val["appid"].asString().c_str());
        strcpy(n.device_id, val["deviceid"].asString().c_str());
        n.online_flag = 0;
        l->push_back(n);
    }
    else if(!strcmp(cmd, "search_bind")){
        //check if app_id exists in the list
        std::list<Node>::iterator it;
        for (it = l->begin(); it != l->end(); it++){
            if(!strcmp(it->app_id, val["appid"].asString().c_str())){
                //update app_bev
                it->app_bev = bev;
                //app_id exists
                val["cmd"] = "reply_bind";
                val["result"] = "yes";
                break;
            }
        }
        if (it == l->end()){      //appid doesn't exist
            val["cmd"] = "reply_bind";
            val["result"] = "no";
        }
        std::string str = Json::FastWriter().write(val);
        size_t ret = bufferevent_write(bev, str.c_str(), strlen(str.c_str()));
        if (ret < 0){
            std::cout << "bufferevent_write error\n";
        }
    }
    else if(!strcmp(cmd, "app_start")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_stop")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_suspend")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_continue")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_prior")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_next")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_volume_up")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_volume_down")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_sequence")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_random")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_circle")){
        p->player_operation(l, bev, cmd);
    }
    else if(!strcmp(cmd, "app_music")){
        p->player_operation(l, bev, cmd);
    }
    //messages from music_player(client)
    else if(!strcmp(cmd, "reply")){
        p->player_reply_result(l, bev, val);
    }
    else if(!strcmp(cmd, "info")){
        p->player_alive_info(l, bev, val, base);
    }
    else if(!strcmp(cmd, "reply_status")){
        p->player_reply_result(l, bev, val);
    }
    else if(!strcmp(cmd, "reply_music")){
        p->player_reply_result(l, bev, val);
    }
}

void PlayerServer::event_cb(struct bufferevent *bev, short wait, void *ctx){
    if(wait & BEV_EVENT_EOF){
        for(std::list<Node>::iterator it = l->begin(); it != l->end(); it++){
            if (it->device_bev == bev){
                std::cout << "music player is off\n";
                it -> online_flag = 0;
                event_del(it->timeout); //delete timer
                return;
            }
        }
        std::cout << "App if off" << std::endl;
    }
    else{
        std::cout << "unexpected error" << std::endl;
    }
}
