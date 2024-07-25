#include <stdio.h>
#include "link.h"
#include "main.h"
#include <stdlib.h> //for malloc
#include <string.h>
#include "player.h"
#include <time.h>

//initialize music list as bidirectional recycle link
int InitLink(){
    head = (Node *)malloc(sizeof(Node)* 1);
    if (head == NULL){
        return FAILURE;
    }
    head->next = head;
    head->prior = head;
    return SUCCESS;
}

int InsertLink(Node *h, const char *name){
    if(h == NULL || name == NULL){
        return FAILURE;
    }

    Node *end = h->prior;

    Node *new = (Node *)malloc(sizeof(Node) * 1);
    if (new == NULL){
        return FAILURE;
    }

    new->next = h;
    strcpy(new->music_name,name);
    end->next = new;
    new->prior = end;
    h->prior = new;

    return SUCCESS;
}

void FindNextMusic(const char *cur, int mode, char *next){ // return the music name
    if(mode == CIRCLEMODE || mode == SEQUENCEMODE){
        Node *p = head->next;

        //find the place of current music in music list
        while(strcmp(p->music_name, cur) != 0){
            p = p->next;
        } 

        //skip the head node (head node contain no music), copy the next music name
        if(p->next == head)
        {
            strcpy(next, head->next->music_name);
        }
        else{
            strcpy(next, p->next->music_name);
        }
        return;
    }
    else{ //random play mode
        Node *p = head->next;
        srand(time(NULL));
        int num = rand() % 100;

        int i;
        for (i = 0; i < num; i++){
            p = p->next;
        }

        //if randomly chosed head node (contain no music), use the next music
        if(p == head)
        {
            strcpy(next, head->next->music_name);
        }
        else{
            strcpy(next, p->music_name);
        }
        return;
    }
}

void FindPriorMusic(const char *cur, int mode, char *prior){ // return the music name
    if(mode == CIRCLEMODE || mode == SEQUENCEMODE){
        Node *p = head->next;
        //find the place of current music in music list
        while(strcmp(p->music_name, cur) != 0){
            p = p->next;
        } 

        //skip head node, find the prior music
        if(p == head->next)
        {
            strcpy(prior, head->prior->music_name);
        }
        else{
            strcpy(prior, p->prior->music_name);
        }
        return;
    }
    else{ //random play mode
        Node *p = head->next;
        srand(time(NULL));
        int num = rand() % 100;

        int i;
        for (i = 0; i < num; i++){
            p = p->next;
        }

        if(p == head)
        {
            strcpy(prior, head->next->music_name);
        }
        else{
            strcpy(prior, p->music_name);
        }
        return;
    }
}