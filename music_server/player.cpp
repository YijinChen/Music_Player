#include "player.h"

void Player::player_alive_info(std::list<Node> *l, struct bufferevent *bev, Json::Value val, struct event_base *base){
    for(std::list<Node>::iterator it = l->begin(); it != l->end(); it++){
        char deviceid[8] = {0};
        strcpy(deviceid, val["deviceid"].asString().c_str());
        for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++){
            if(!strcmp(deviceid, it->device_id)){
                if(it->online_flag == 0){    //It's the first time to send keep_alive                   
                    //Set event parameter
                    //EV_PERSIST: keep the event recycle
                    std::cout << "Entering event_assgin ..\n";
                    event_assign(it->timeout, base, -1, EV_PERSIST, timeout_cb, bev); 
                    
                    std::cout << "Finished event_assgin ..\n";
                    struct timeval tv;
                    evutil_timerclear(&tv);
                    tv.tv_sec = 1; // run 1 time per 1 second
                    event_add(it->timeout, &tv);
                    it->online_flag = 1;
                }
                it->device_bev = bev;
                it->time = time(NULL);
                std::cout << "received keep alive info, successfully updated info" << std::endl;
                return;
            }
        }
    }
    std::cout << "received keep alive info, but the app is not bound yet" << std::endl;
}

//server received command from app, then the server transfer the command to music_player
//app_bev is app's bufferevent
void Player::player_operation(std::list<Node> *l, struct bufferevent *app_bev, const char* cmd){
    Json::Value val;
    if(!strcmp(cmd, "app_start")){
        val["cmd"] = "start";
    }
    else if(!strcmp(cmd, "app_stop")){
        val["cmd"] = "stop";
    }
    if(!strcmp(cmd, "app_start")){
        val["cmd"] = "start";
    }
    else if(!strcmp(cmd, "app_suspend")){
        val["cmd"] = "suspend";
    }
    if(!strcmp(cmd, "app_continue")){
        val["cmd"] = "continue";
    }
    else if(!strcmp(cmd, "app_prior")){
        val["cmd"] = "prior";
    }
    if(!strcmp(cmd, "app_next")){
        val["cmd"] = "next";
    }
    else if(!strcmp(cmd, "app_voice_up")){
        val["cmd"] = "voice_up";
    }
    if(!strcmp(cmd, "app_voice_down")){
        val["cmd"] = "voice_down";
    }
    else if(!strcmp(cmd, "app_sequence")){
        val["cmd"] = "sequence";
    }
        if(!strcmp(cmd, "app_random")){
        val["cmd"] = "random";
    }
    else if(!strcmp(cmd, "app_circle")){
        val["cmd"] = "circle";
    }

    std::string str = Json::FastWriter().write(val); //convert val to string
    size_t ret;
    //check list, check if the device is online
    for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++){
        if (it->app_bev == app_bev){  //if the device exists
            if(it->online_flag == 1){  //if the device is online
                ret = bufferevent_write(it->device_bev, str.c_str(),strlen(str.c_str()));
                if(ret < 0){
                    std::cout << "bufferevent_write error\n";
                }
            }
            else{   // if the device if offline
                Json::Value v;
                v["cmd"] = "app_reply";
                v["result"] = "off_line";
                std::string s = Json::FastWriter().write(v);
                ret = bufferevent_write(app_bev, s.c_str(), strlen(s.c_str()));
                if(ret < 0){
                    std::cout << "bufferevent_write error\n";
                }
            }
        }
    }
}

//server received reply from music_player, then the server will send info to app
//bev is music_pkayer's bufferevent
void Player::player_reply_result(std::list<Node> *l, struct bufferevent *bev, Json::Value val){
    char cmd[32] = {0};
    strcpy(cmd, val["cmd"].asString().c_str());
    if(!strcmp(cmd, "reply")){
        val["cmd"] = "app_reply";
    }
    else if (!strcmp(cmd, "reply_music")){
        val["cmd"] = "app_reply_music";
    }
    else if (!strcmp(cmd, "reply_status")){
        val["cmd"] = "app_reply";
    }

    std::string str = Json::FastWriter().write(val);
    size_t ret;
    //check the list, find the bufferevent corrresponding to app
    for(std::list<Node>::iterator it = l->begin(); it != l->end(); it++){
        if(it->device_bev == bev){
            ret = bufferevent_write(it->app_bev, str.c_str(), strlen(str.c_str()));
            if(ret < 0){
                std::cout << "bufferevent_write error\n";
            }
            return;
        }
    }
    std::cout << "app doesn't exist.\n";
}


void Player::timeout_cb(evutil_socket_t fd, short event, void *arg){
    std::cout << "timer event\n";
    struct bufferevent *bev = (struct bufferevent *)arg;
    Json::Value val;
    val["cmd"] = "get";
    std::string str = Json::FastWriter().write(val);

    size_t ret = bufferevent_write(bev, str.c_str(), strlen(str.c_str()));
    if (ret < 0 ){
        std::cout << "bufferevent_write error\n";
    }

}