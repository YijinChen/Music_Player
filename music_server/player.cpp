#include "player.h"

void Player::player_alive_info(std::list<Node> *l, struct bufferevent *bev, Json::Value val){
    for(std::list<Node>::iterator it = l->begin(); it != l->end(); it++){
        char deviceid[8] = {0};
        strcpy(deviceid, val["deviceid"].asString().c_str());
        for (std::list<Node>::iterator it = l->begin(); it != l->end(); it++){
            if(!strcmp(deviceid, it->device_id)){
                it->device_bev = bev;
                it->online_flag = 1;
                it->time = time(NULL);

                std::cout << "received keep alive info, successfully updated info" << std::endl;
                return;
            }
        }
    }
    std::cout << "received keep alive info, but the app is not binded yet" << std::endl;
}