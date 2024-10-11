#include <stdio.h>
#include "player.h"
#include "link.h"
#include <dirent.h> // for opendir() and readdir()
#include <stdlib.h> //for exit()
#include "main.h"
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h> //for fork()
#include <sys/wait.h> //for wait()
#include <errno.h>
#include <signal.h> //for kill()
//#include "select.h"
#include "socket.h"
#include <sys/ioctl.h>
#include <alsa/asoundlib.h>
//#include <linux/soundcard.h>
#include <sys/types.h>
#include <time.h>
#include <termios.h>


struct Node *head;
void *g_addr = NULL; //map address for shared memory 
int g_start_flag = 0; //if the player is playing music
int g_suspend_flag = 0; //if the player suspend
int init_mixer_flag = 0; //if the mixer successfully initalized
char suspend_name[64];

//mixer
snd_mixer_t *handle;
snd_mixer_selem_id_t *sid;
snd_mixer_elem_t *elem;
long min_volume, max_volume;
long current_volume;
snd_mixer_selem_channel_id_t channel = SND_MIXER_SCHN_FRONT_LEFT;

//for realizing suspend/continue function
time_t start_time, suspend_time;
//for avoiding terminal bug
struct termios old_terminal_settings;


//initialize shared memory
int InitShm(){
    int shmid = shmget(SHMKEY, SHMSIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1) {
        if (errno == EEXIST) {
            // If the segment already exists, try to access the existing segment
            fprintf(stderr, "shmget failed: Segment already exists. Trying to access existing segment.\n");
            shmid = shmget(SHMKEY, SHMSIZE, 0666);
            if (shmid == -1) {
                fprintf(stderr, "shmget failed to access existing segment: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "shmget failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    printf("Shared memory segment created or accessed with ID: %d\n", shmid);

    //map
    g_addr = (char *)shmat(shmid, NULL, 0);
    if (g_addr == (char *)-1) {
        fprintf(stderr, "shmat failed: %s\n", strerror(errno));
        // Clean up the shared memory segment if needed
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            fprintf(stderr, "shmctl(IPC_RMID) failed: %s\n", strerror(errno));
        }
        exit(EXIT_FAILURE);
        return FAILURE;
    }
    printf("Attached to shared memory segment at address: %p\n", g_addr);

    //initialize shared memory information
    shm s;
    s.play_mode = SEQUENCEMODE;
    s.ppid = getpid();

    memcpy(g_addr, &s, sizeof(s));
    
    return SUCCESS;
}

//judge if the file name is end with .mp3
int m_mp3_end(const char *name){

    const char *ptr = name;
    while (*ptr != '\0'){
        ptr++;
    }

    int i;
    for(i = 0; i < 4; i++){
        ptr--;
    }

    if(ptr < name)
        return 0;

    return (strcmp(ptr, ".mp3") == 0) ? 1 : 0;
}

void GetMusic(){
    //open directory
    DIR *dir = opendir(MUSICPATH);
    if(dir == NULL){
        perror("opendir");
        exit(1);
    }

    //read directory
    struct dirent *file;
    while((file = readdir(dir)) != NULL){
        if(file->d_type != 8){ // if file is a normal file
            continue;
        }

        if (!m_mp3_end(file->d_name)){ // if file is not .mp3
            continue;
        }

        printf("%s\n", file->d_name);

        int ret;
        ret = InsertLink(head, file->d_name);
        if (ret == FAILURE){
            printf("fail to insert music\n");
        }
    }
}

void play_music(char *name, int skip_frames){
    ShowLink();
    pid_t control_pid = fork();
    if (control_pid == -1){
        perror("fork");
        exit(1);
    }
    else if (control_pid == 0){ //create subprocess
        pid_t music_pid = fork();
        if(music_pid == -1){
            perror("fork");
            exit(1); // exit the subprocess
        }
        else if (music_pid == 0){ // create sub-subprocess
            char skip_arg[20];
            shm s;
            char cur_name[64] = {0}; // name for the current music
            //get shared memory
            int shmid = shmget(SHMKEY, SHMSIZE, 0);
            if(shmid == -1){
                perror("shmget");
                exit(1);
            }

            //map
            void *addr = shmat(shmid, NULL, 0);
            if (addr == NULL){
                perror("shmat");
                exit(1);
            }

            if(strlen(name) != 0){ //when name is refered by function parameter, play the music
                strcpy(cur_name, name);
            }else{ //when name is emptyed by subprocess, find the next music, then play it
                memcpy(&s, addr, sizeof(s));
                FindNextMusic(s.cur_name, s.play_mode, cur_name);
            }

            //write information into shared memory: all the process ids, current music name
            memcpy(&s, addr, sizeof(s)); // read info from shared memory addr into s
            strcpy(s.cur_name, name); // change some info of s
            s.control_pid = getppid();
            s.music_pid = getpid();
            memcpy(addr, &s, sizeof(s)); // write info back to shared memory addr from s

            shmdt(addr); //cancel the map

            char music_path[128] = {0};
            strcpy(music_path, MUSICPATH);
            strcat(music_path, cur_name);

            printf("Play Music: %s\n", music_path);
            sprintf(skip_arg, "-k %d", skip_frames);
            execl("/usr/bin/mpg123", "mpg123", "-q", skip_arg, music_path, NULL);
        }
        else{ //create subprocess
            memset(name, 0, strlen(name)); //empty the name, wait for next usage
            int status;
            waitpid(music_pid, &status, 0); //recycle sub-subprocess
        }
        return;
    }
    else{
        return;
    }
}

void start_play(char *name){
    if (g_start_flag == 1){ // if the player is playing
        return;
    }
    //Get music name
    if(head->next == NULL){ //if music list is empty
        return;
    }
    // Get the initial volume
    init_mixer();
    long volume;
    volume = get_volume();
    printf("Current volume: %ld%%\n", volume);

    //set default mode to sequence
    set_mode(SEQUENCEMODE);

    //char name[128] = {0};
    strcpy(name, head->next->music_name);
    start_time = time(NULL);  // Record start time when music starts

    // Save current terminal settings and set cleanup on exit
    tcgetattr(STDIN_FILENO, &old_terminal_settings);
    atexit(restore_terminal_settings);
    signal(SIGINT, handle_exit_signal);
    signal(SIGTERM, handle_exit_signal);

    //start to play music
    play_music(name, 0); 
    g_start_flag = 1;
}

void stop_play(){
    if(g_start_flag == 0){
        return;
    }
    
    //read shared memory for pids
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    kill(s.control_pid, SIGKILL);//kill subprocess
    kill(s.music_pid, SIGKILL);//kill sub-subprocess
    snd_mixer_close(handle);//close mixer

    g_start_flag = 0;
}

void suspend_play(){
    if(g_start_flag == 0 || g_suspend_flag == 1){
        return;
    }
    printf("Suspending music...\n");
    //read shared memory for pid    
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));
    strcpy(suspend_name, s.cur_name);

    // Attempt to suspend the subprocess
    kill(s.control_pid, SIGSTOP);
    // Attempt to suspend the sub-subprocess
    if (kill(s.music_pid, SIGSTOP) == 0) {
        //printf("Successfully suspended music_process (PID: %d)\n", s.music_pid);
        suspend_time = time(NULL);  // Record the time when music was suspended
    } else {
        perror("Failed to suspend music\n");
    }
    g_suspend_flag = 1;
}

void continue_play(){
    if(g_start_flag == 0 || g_suspend_flag == 0){
        return;
    }
    printf("Resuming music...\n");
    //read shared memory for pid    
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    // Calculate the total time in seconds that the music played before suspension
    int total_skip_seconds = difftime(suspend_time, start_time);

    //continue control process
    kill(s.control_pid, SIGCONT);
    // Kill the old mpg123 process
    kill(s.music_pid, SIGKILL);

    // Convert total time into frames (approx. 38.28 frames per second)
    double frame_rate = 38.25;
    int skip_frames = (int)(total_skip_seconds * frame_rate);
    // Start mpg123 again, skipping the calculated number of frames
    play_music(suspend_name, skip_frames);

    g_suspend_flag = 0;
}

void prior_play(char *name){
    if(g_start_flag == 0){
        return;
    }

    //read shared memory for pids
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    kill(s.control_pid, SIGKILL);//kill subprocess
    kill(s.music_pid, SIGKILL);//kill sub-subprocess

    g_start_flag = 0;
    //char name[64] = {0};
    FindPriorMusic(s.cur_name, s.play_mode, name);
    play_music(name, 0);

    g_start_flag = 1;
}

void next_play(char *name){
    if(g_start_flag == 0){
        return;
    }

    //read shared memory for pids
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    kill(s.control_pid, SIGKILL);//kill subprocess
    kill(s.music_pid, SIGKILL);//kill sub-subprocess

    g_start_flag = 0;
    //char name[64] = {0};
    FindNextMusic(s.cur_name, s.play_mode, name);
    play_music(name, 0);

    g_start_flag = 1;
}

int init_mixer(){
    // Open the mixer
    if (snd_mixer_open(&handle, 0) < 0) {
        fprintf(stderr, "Error: Unable to open mixer\n");
        return -1;
    }
    // Attach to the default card
    if (snd_mixer_attach(handle, "default") < 0) {
        fprintf(stderr, "Error: Unable to attach to default card\n");
        snd_mixer_close(handle);
        return -1;
    }
    // Register the mixer
    if (snd_mixer_selem_register(handle, NULL, NULL) < 0) {
        fprintf(stderr, "Error: Unable to register mixer\n");
        snd_mixer_close(handle);
        return -1;
    }
    // Load mixer elements
    if (snd_mixer_load(handle) < 0) {
        fprintf(stderr, "Error: Unable to load mixer\n");
        snd_mixer_close(handle);
        return -1;
    }
    // Allocate memory for mixer simple element ID
    snd_mixer_selem_id_alloca(&sid);
    // Set the mixer element name (e.g., "Master")
    snd_mixer_selem_id_set_name(sid, MIXER_NAME);
    // Find the mixer element
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        fprintf(stderr, "Error: Unable to find mixer element\n");
        snd_mixer_close(handle);
        return -1;
    }
    // Get the volume range (min and max)
    snd_mixer_selem_get_playback_volume_range(elem, &min_volume, &max_volume);
    // initialize current_volume
    snd_mixer_selem_get_playback_volume(elem, channel, &current_volume);
    init_mixer_flag = 1;
    return 1;
}

// Function to get the current volume percent
long get_volume() {
    if(init_mixer_flag == 1){
        return (current_volume * 100) / max_volume;
    }
    else{
        return 0;
    }
}

// Function to set the volume
void set_volume(long volume_percent) {
    // Calculate the actual volume value based on the percentage
    long volume = (volume_percent * max_volume) / 100;
    // Set the volume
    if (snd_mixer_selem_set_playback_volume_all(elem, volume) < 0) {
        fprintf(stderr, "Error: Unable to set volume\n");
    }
    current_volume = volume;
}

void voice_up(){
    long volume;
    volume = get_volume();
    volume += 6;  // Increase volume by 5%
    if (volume > 100) volume = 100;  // Limit to 100%
    set_volume(volume);  // Set volume as a percentage
    printf("Volume increased to: %ld%%\n", volume);
}

void voice_down(){
    long volume;
    volume = get_volume();
    volume -= 4;  // Decrease volume by 5%
    if (volume < 0) volume = 0;  // Limit to 0%
    set_volume(volume);  // Set volume as a percentage
    printf("Volume decreased to: %ld%%\n", volume);
}

void set_mode(int mode){
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    s.play_mode = mode;
    memcpy(g_addr, &s, sizeof(s));
    char mode_name[64];
    mode_to_string(mode, mode_name);
    printf("successfully change mode to %s!\n", mode_name);
}

void restore_terminal_settings() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal_settings);
}

void handle_exit_signal(int signum) {
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));
    if (s.music_pid != 0) {
        kill(s.music_pid, SIGKILL);
    }
    restore_terminal_settings();
    printf("\nMusic player stopped due to signal %d. Exiting...\n", signum);
    exit(0);
}

void mode_to_string(int mode, char* mode_name){
    switch(mode){
    case 1:
        strcpy(mode_name, "Sequance Mode");
        break;
    case 2:
        strcpy(mode_name, "Random Mode");
        break;
    case 3:
        strcpy(mode_name, "Circle Mode");
        break;
    }
}