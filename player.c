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

extern Node *head;
void *g_addr = NULL; //map address for shared memory 
int g_start_flag = 0; //if the player is playing music
int g_suspend_flag = 0; //if the player suspend

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

void play_music(char *name){
    pid_t child_pid = fork();
    if (child_pid == -1){
        perror("fork");
        exit(1);
    }
    else if (child_pid == 0){ //create subprocess
        while(1){
            pid_t grand_pid = fork();
            if(grand_pid == -1){
                perror("fork");
                exit(1); // exit the subprocess
            }
            else if (grand_pid == 0){ // create sub-subprocess
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
                    //judge play mode and fine next music
                    memcpy(&s, addr, sizeof(s));
                    FindNextMusic(s.cur_name, s.play_mode, cur_name);
                }
                //write information into shared memory: all the process ids, current music name
                memcpy(&s, addr, sizeof(s)); // read info from shared memory addr into s
                strcpy(s.cur_name, name); // change some info of s
                s.child_pid = getppid();
                s.grand_pid = getpid();
                memcpy(addr, &s, sizeof(s)); // write info back to shared memory addr from s
                shmdt(addr); //cancel the map

                char music_path[128] = {0};
                strcpy(music_path, MUSICPATH);
                strcat(music_path, cur_name);
                printf("Play Music: %s\n", music_path);
                execl("/usr/bin/afplay", "afplay", music_path, NULL); 
            }
            else{ //create subprocess
                memset(name, 0, strlen(name)); //empty the name, wait for next usage

                int status;
                waitpid(grand_pid, &status, 0); //recycle sub-subprocess
            }
        }
    }
    else{
        return;
    }

}

void start_play(){
    if (g_start_flag == 1){ // if the player is playing
        return;
    }

    //Get music name
    if(head->next == NULL){ //if music list is empty
        return;
    }

    //start to play music
    char name[128] = {0};
    strcpy(name, head->next->music_name);
    play_music(name);
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

    kill(s.child_pid, SIGKILL);//kill subprocess
    kill(s.grand_pid, SIGKILL);//kill sub-subprocess

    g_start_flag = 0;
}

void suspend_play(){
    if(g_start_flag == 0 || g_suspend_flag == 1){
        return;
    }

    //read shared memory for pid    
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    kill(s.grand_pid, SIGSTOP);//suspend sub-subprocess
    kill(s.child_pid, SIGSTOP);//suspend subprocess

    g_suspend_flag = 1;
}

void continue_play(){
    if(g_start_flag == 0 || g_suspend_flag == 0){
        return;
    }

    //read shared memory for pid    
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    kill(s.grand_pid, SIGCONT);//suspend sub-subprocess
    kill(s.child_pid, SIGCONT);//suspend subprocess

    g_suspend_flag = 0;
}

void prior_play(){
    if(g_start_flag == 0){
        return;
    }

    //read shared memory for pids
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    kill(s.child_pid, SIGKILL);//kill subprocess
    kill(s.grand_pid, SIGKILL);//kill sub-subprocess

    g_start_flag = 0;
    char name[64] = {0};
    FindPriorMusic(s.cur_name, s.play_mode, name);
    play_music(name);

    g_start_flag = 1;
}

void next_play(){
    if(g_start_flag == 0){
        return;
    }

    //read shared memory for pids
    shm s;
    memset(&s, 0, sizeof(s));
    memcpy(&s, g_addr, sizeof(s));

    kill(s.child_pid, SIGKILL);//kill subprocess
    kill(s.grand_pid, SIGKILL);//kill sub-subprocess

    g_start_flag = 0;
    char name[64] = {0};
    FindNextMusic(s.cur_name, s.play_mode, name);
    play_music(name);

    g_start_flag = 1;
}