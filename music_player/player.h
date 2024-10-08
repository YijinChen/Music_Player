#ifndef PLAYER_H
#define PLAYER_H
#include <unistd.h> //for pid_t

//#define MUSICPATH "/home/ubuntu/playground/Music_Player/music_list/" //for ubuntu
///usr/share/myir/Music
#define MUSICPATH "/Users/yijin/Documents/Cpp_Projects/Music_Player/short_music_list/" //for mac
#define SHMKEY 0x1234
#define SHMSIZE 4096
void GetMusic();
int InitShm();
void start_play();
void stop_play();
void suspend_play();
void continue_play(); 
void prior_play();
void next_play();
void voice_up();
void voice_down();
void set_mode(int mode);

#define SEQUENCEMODE    1
#define RANDOMMODE      2
#define CIRCLEMODE      3

//shared memory infomation
struct shm{
    int play_mode;
    char cur_name[64];
    pid_t ppid;
    pid_t child_pid;
    pid_t grand_pid;
};
typedef struct shm shm;
extern int iLeft;
extern int g_start_flag;
extern int g_suspend_flag;
extern void *g_addr;

#endif

