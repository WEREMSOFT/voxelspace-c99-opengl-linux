#include "stack_allocator.h"
#include <stdlib.h>

StackAllocator *stackAllocatorCreate(size_t size)
{
    size_t sizeToAllocate = size + sizeof(StackAllocatorHeader);
    StackAllocator *this = (StackAllocator *)calloc(sizeof(char), sizeToAllocate);

    if (!this)
    {
        printf("Error creating stack allocator %s::%d\n", __FILE__, __LINE__);
        return NULL;
    }

    this->header.capacity = size;
    return this;
}

void *stackAllocatorAlloc(StackAllocator *this, size_t size)
{
    if (this->header.length + size > this->header.capacity)
    {
        // TODO: realloc will invalidate all the pointers provided before.
        // For now, we just abort the program, in the future we should create
        // A handle table
        printf("Allocator ran out of space.\nActual capacity %ld\nRequired capacity %ld\n", this->header.capacity, this->header.capacity + size);
        return NULL;
    }
    void *returnValue = &this->data[this->header.length];
    this->header.length += size;
    this->header.allocationCount++;
    return returnValue;
}

void stackAllocatorReset(StackAllocator *this)
{
    this->header.length = 0;
}

void stackAllocatorFree(StackAllocator *this)
{
    this->header.allocationCount--;
    if (this->header.allocationCount <= 0)
    {
        stackAllocatorReset(this);
    }
}

void *stackAllocatorRealloc(StackAllocator *this, void *pointer, size_t size)
{
    void *returnValue = stackAllocatorAlloc(this, size);

    if (pointer != NULL)
        memcpy(returnValue, pointer, size);

    return returnValue;
}