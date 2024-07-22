#ifndef LINK_H
#define LINK_H

struct Node{
    char music_name[64];
    struct Node *next;
};

typedef struct Node Node;
int InitLink();

#endif