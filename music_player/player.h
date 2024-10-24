#ifndef PLAYER_H
#define PLAYER_H
#include <unistd.h> //for pid_t
#include <time.h>

#define MUSICPATH "/usr/share/myir/Music/"
#define MIXER_NAME "Headphone"
#define SHMKEY 0x1234
#define SHMSIZE 4096
void GetMusic();
int InitShm();
void start_play(char *name);
void stop_play();
void suspend_play();
void resume_play(); 
void previous_play(char *name);
void next_play(char *name);
void volume_up();
void volume_down();
void set_mode(int mode);
int init_mixer();
long get_volume();
void set_volume(long volume_percent);
void restore_terminal_settings();
void handle_exit_signal(int signum);
void play_music(char *name, int skip_frames);
void mode_to_string(int mode, char* mode_name);
void player_change_mode();

#define SEQUENCEMODE    1
#define RANDOMMODE      2
#define CIRCLEMODE      3
#define TOTALMODE     3

//shared memory infomation
struct shm{
    int play_mode;
    char cur_name[64];
    pid_t ppid;
    pid_t control_pid;
    pid_t music_pid;
    int skip_seconds;
    time_t start_time;
};
typedef struct shm shm;
//extern int iLeft;
extern int g_start_flag;
extern int g_suspend_flag;
extern void *g_addr;
extern long current_volume;


#endif

