#ifndef LINK_H
#define LINK_H

struct Node{
    char music_name[64];
    struct Node *next;
    struct Node *prior;
};

typedef struct Node Node;
int InitLink();
int InsertLink(Node *h, const char *name);
void FindnextMusic(const char *cur, int mode, char *next);

#endif