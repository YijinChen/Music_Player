#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>  // For fd_set and other types

#define SERVER_PORT 8000
#define SERVER_IP "18.185.92.160"//the public address of server
//#define SERVER_IP "127.0.0.1"//ip addres of the host itself
#define TIMEOUT 1

void send_server(int sig);
void *connect_cb(void *arg);
int InitSocket();
void socket_start_play();
void socket_stop_play();
void socket_suspend_play();
void socket_continue_play();
void socket_prior_play();
void socket_next_play();
void socket_volume_up_play();
void socket_volume_down_play();
void socket_mode_play(int mode);
void socket_get_status();
void socket_get_music();

extern int g_sockfd;
extern fd_set readfd;
extern int g_maxfd;

#endif