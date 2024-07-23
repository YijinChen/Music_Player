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

extern Node *head;
void *g_addr = NULL; //map address for shared memory 
int g_start_flag = 0; //if the player is playing music
int g_suspend_flag = 0; //if the player suspend

//initialize shared memory
int InitShm(){
    int shmid = shmget(SHMKEY, SHMSIZE, IPC_CREAT | IPC_EXCL);
    if(shmid == -1){
        return FAILURE;
    }
    //map
    g_addr = shmat(shmid, NULL, 0);
    if(g_addr == NULL){
        return FAILURE;
    }
    //initialize shared memory information
    shm s;
    s.play_mode = SEQUENCEMODE;
    s.ppid = getpid();

    memecpy((char *)g_addr, &s, sizeof(s));
    
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

void play_music(const char *name){
    pid_t child_pid = fork();
    if (child_pid == -1){
        perror("fork");
        exit(1);
    }
    else if (child_pid == 0){ //create subprocess
        while(1){
            pid_t grand_pid = vfork();
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
                    FindnextMusic(s.cur_name, s.play_mode, cur_name);
                }
                //write information into shared memory: all the process ids, current music name
                strcpy(s.cur_name, name);
                s.child_pid = getppid();
                s.grand_pid = getpid();
                memcpy(addr, &s, sizeof(s));
                stmdt(addr); //cancel the map

                char music_path[128] = {0};
                strcpy(music_path, MUSICPATH);
                strcat(music_path, cur_name);
                execl("usr/.../madplay", "madplay", music_path, NULL); 
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