#ifndef __STACK_ALLOCATOR_H__
#define __STACK_ALLOCATOR_H__

/**
 * StackAllocator is a custom allocator that returns pointers to a memory arena and grows itself 
 * when is asked to reserve more memory than capacity.
 * 
 * Using this type of custom allocator has the following advantages over using malloc and free
 * in a clasical way:
 * 
 * 1. Allocate memory and free memory takes time because the operative system need to do bookkeeping
 * of the memory that was allocated. Using a StackAllocator with a reasonable gessed initial size
 * will avoid the cycle of alloc/free removing the performance penalty.
 * 
 * 2. Memory fragmentation is reduced, specially when working with a lot of small objects.
 * 
 * 3. You can handle process that requires allocate and free memory in a more maintainable way
 * you create the StackAllocator in at the start of the process(let's say, in the begining of the
 * asset loading routine) allocate how many objects you need, and they you free only the allocator.
 * Reducing the stress to the operative system and the memory fragmentation for your program.
 * 
 */

#include <string.h>
#include <stdio.h>

typedef struct
{
    size_t capacity;
    size_t length;
    int allocationCount;
} StackAllocatorHeader;

typedef struct
{
    StackAllocatorHeader header;
    char data[1];
} StackAllocator;

StackAllocator *stackAllocatorCreate(size_t size);
void *stackAllocatorAlloc(StackAllocator *this, size_t size);
void stackAllocatorReset(StackAllocator *this);
void stackAllocatorFree(StackAllocator *this);
void *stackAllocatorRealloc(StackAllocator *this, void *pointer, size_t size);

#endif