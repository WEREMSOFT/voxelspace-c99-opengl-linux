#ifndef __STATIC_ALLOC_H__
#define __STATIC_ALLOC_H__
#include "stack_allocator.h"
#include <stdlib.h>

void *allocStatic(size_t size);
void *reallocStatic(void *this, size_t size);
void freeStatic(void *this);
void staticAllocatorDestroy();
#endif

#ifdef __STATIC_ALLOC_IMPLEMENTATION__
#undef __STATIC_ALLOC_IMPLEMENTATION__

static StackAllocator *stackAllocator;

void staticAllocatorInit(long int capacity)
{
    stackAllocator = stackAllocatorCreate(capacity);
}

void *allocStatic(size_t size)
{
    void *returnValue = stackAllocatorAlloc(stackAllocator, size);
    return returnValue;
}
void *reallocStatic(void *this, size_t size)
{
    void *returnValue = stackAllocatorRealloc(stackAllocator, this, size);
    return returnValue;
}

void freeStatic(void *this)
{
    if (this == NULL)
        return;
    stackAllocatorFree(stackAllocator);
}

void staticAllocatorDestroy()
{
    printf("Total memory used %ld out of %ld\n", stackAllocator->header.length, stackAllocator->header.capacity);
    free(stackAllocator);
}

#endif