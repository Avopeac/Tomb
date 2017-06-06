#pragma once

#include "SDL.h"

#include "TOMB_defines.h"

// Memory allocators

#ifndef TOMB_UNDEFINED_ALLOCATOR
#define TOMB_UNDEFINED_ALLOCATOR 0x00
#endif

#ifndef TOMB_BUDDY_ALLOCATOR
#define TOMB_BUDDY_ALLOCATOR 0x02
#endif

#ifndef TOMB_BUDDY_ALLOCATOR_MAX_LEVELS
#define TOMB_BUDDY_ALLOCATOR_MAX_LEVELS 32
#endif 

struct TOMB_BuddyAllocator
{
	Uint8 type = TOMB_BUDDY_ALLOCATOR;
	Uint64 size = 0;
	Uint32 levels = 0;
	void * pMemory = 0;
	void * pFreeBlocks[TOMB_BUDDY_ALLOCATOR_MAX_LEVELS];
};

struct TOMB_MemoryAllocation
{
	
	Uint8 type = TOMB_UNDEFINED_ALLOCATOR;
	Uint64 size = 0;
	void * pMemory = 0;

	union
	{
		TOMB_BuddyAllocator * pBuddyAllocator = 0;
	};
};


TOMB_INTERNAL Uint32 HighestBit(Uint64 number)
{
	Uint32 bitCount = 0;
	while ((number >>= 1) != 0)
	{
		++bitCount;
	}

	return ++bitCount;
}

Sint32 TOMB_MemoryInitializeAllocator(TOMB_BuddyAllocator * pAllocator, Uint64 size)
{
	SDL_assert(pAllocator);

	pAllocator->pMemory = SDL_malloc(size);

	if (!pAllocator->pMemory)
	{
		return TOMB_FAILURE;
	}

	pAllocator->size = size;
	pAllocator->levels = HighestBit(size);

	return TOMB_SUCCESS;
}

Sint32 TOMB_MemoryDestroyAllocator(TOMB_BuddyAllocator * pAllocator)
{
	return TOMB_SUCCESS;
}