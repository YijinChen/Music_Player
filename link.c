#include <stdio.h>
#include "link.h"
#include "main.h"
#include <stdlib.h> //for malloc
#include <string.h>
extern struct Node *head;

//initialize music list
int InitLink(){
    head = (Node *)malloc(sizeof(Node)* 1);
    if (head == NULL){
        return FAILURE;
    }
    head->next = NULL;
    return SUCCESS;
}

int InsertLink(Node *h, const char *name){
    if(h == NULL || name == NULL){
        return FAILURE;
    }

    Node *p = h;

    while(p->next){
        p = p->next;
    }

    Node *new = (Node *)malloc(sizeof(Node) * 1);
    if (new == NULL){
        return FAILURE;
    }

    new->next = NULL;
    strcpy(new->music_name,name);
    p->next = new;

    return SUCCESS;
}