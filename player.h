#ifndef PLAYER_H
#define PLAYER_H
#include <unistd.h> //for pid_t

//#define MUSICPATH "/home/ubuntu/playground/Music_Player/music_list/" //for ubuntu
#define MUSICPATH "/Users/yijin/Documents/Cpp Projects/Music_Player/music_list" //for mac
#define SHMKEY 1234
#define SHMSIZE 4096
void GetMusic();
int InitShm();

#define SEQUENCEMODE    1
#define RANDOM          2
#define CIRCLE          3

//shared memory infomation
struct shm{
    int play_mode;
    char cur_name[64];
    pid_t ppid;
    pid_t child_pid;
    pid_t grand_pid;

};
typedef struct shm shm;

#endif

