#include <stdio.h>
#include "player.h"
#include "link.h"
#include <dirent.h> // for opendir() and readdir()
#include <stdlib.h> //for exit()
#include "main.h"
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>

extern Node *head;
void *g_addr = NULL; //map address for shared memory 

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

void start_play(){

}