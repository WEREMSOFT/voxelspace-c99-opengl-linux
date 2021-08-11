#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>
#include <stdlib.h>

struct Node;

typedef struct Node
{
    void *data;
    struct Node *next;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
} Queue;

bool queueIsEmpty(Queue this);
Node *nodeCreate(void *data);
Queue queueEnqueue(Queue this, void *data);
void *queueDequeue(Queue *this);
void queueDestroy(Queue *this);

#endif