#include "queue.h"

bool queueIsEmpty(Queue this)
{
    return this.head->next == NULL;
}

Node *nodeCreate(void *data)
{
    Node *this = malloc(sizeof(Node));
    this->data = data;
    return this;
}

Queue queueEnqueue(Queue this, void *data)
{
    Node *node = nodeCreate(data);
    if (this.head == NULL)
    {
        this.head = node;
        this.tail = this.head;
        this.head->next = this.tail;
    }
    else
    {
        this.tail->next = node;
        this.tail = node;
    }
    return this;
}

void *queueDequeue(Queue *this)
{
    if (this->head == NULL)
        return NULL;

    Node *headNode = this->head;
    void *data = headNode->data;

    this->head = this->head->next;

    if (headNode)
    {
        free(headNode);
    }

    return data;
}

void queueDestroy(Queue *this)
{
    Node *node;
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
