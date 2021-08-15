#include "queue.h"
#include <pthread.h>

bool queueIsEmpty(Queue this)
{
    return this.head->next == NULL;
}

Node *nodeCreate(void *data)
{
    Node *this = malloc(sizeof(Node));
    this->data = data;
    this->next = this;
    return this;
}

void queueEnqueue(Queue *this, void *data)
{

    Node *node = nodeCreate(data);
    if (this->head == NULL)
    {
        this->head = node;
        this->tail = this->head;
        this->head->next = this->tail;
    }
    else
    {
        this->tail->next = node;
        this->tail = node;
    }
}

void *queueDequeue(Queue *this)
{
    if (this->head == NULL)
        return NULL;

    void *data;

    Node *headNode = this->head;
    data = headNode->data;

    if (this->head == this->head->next)
    {
        this->head = NULL;
        this->tail = NULL;
    }
    else
    {
        this->head = this->head->next;
    }

    if (headNode)
    {
        free(headNode);
    }

    return data;
}

void queueDestroy(Queue *this)
{
    while (this->head)
    {
        Node *oldHead = this->head;
        if (this->head->next == this->head)
        {
            this->head->next = NULL;
        }
        this->head = this->head->next;
        free(oldHead);
    }

    this->tail = NULL;
}
