#include <stdio.h>
#include "link.h"
#include "main.h"
#include <stdlib.h> //for malloc
#include <string.h>
#include "player.h"
#include <time.h>
extern struct Node *head;

//initialize music list as bidirectional recycle link
int InitLink(){
    head = (Node *)malloc(sizeof(Node)* 1);
    if (head == NULL){
        return FAILURE;
    }
    head->next = head;
    head->prior =head;
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

void FindnextMusic(const char *cur, int mode, char *next){ // return the music name
    if(mode == CIRCLE){
        strcpy(next, cur);
    }
    else if(mode == SEQUENCEMODE){
        Node *p = head->next;
        while(strcmp(p->music_name, cur) != 0){
            p = p->next;
        }
        strcpy(next, p->next->music_name);
        return;
    }
    else{
        Node *p = head->next;
        srand(time(NULL));
        int num = rand() % 100;

        int i;
        for (i = 0; i < num; i++){
            p = p->next;
        }

        strcpy(next, p->music_name);

        return;
    }



}