#ifndef NODE_H
#define NODE_H
#include <event2/event.h>
#include <time.h>

struct Node{
    struct bufferevent *app_bev;
    struct bufferevent *device_bev;
    char app_id[8];
    char device_id[8];
    int online_flag;  //represents if this device is online, 0-offline, 1-online
    int app_online_flag;  //represents if app is online, 0-offline, 1-online
    time_t time;  //time when received "keep alive" message
    struct event *timeout;  //use for peridically sending get_status info from app to client
};
typedef struct Node Node;

#endif