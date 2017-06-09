#pragma once

#include "SDL.h"

#include "TOMB_defines.h"

struct TOMB_Memory
{
	void * pPersistentStorage;
	void * pTransientStorage;
	Uint64 PersistentStorageSize;
	Uint64 TransientStorageSize;
};

struct TOMB_MemoryArena
{
	Uint8 * Address;
	Uint64 Size;
	Uint64 Occupied;
};

void TOMB_MemoryArenaInitialize(TOMB_MemoryArena * pArena, Uint8 * pAddress, Uint64 size)
{
	pArena->Address = pAddress;
	pArena->Size = size;
	pArena->Occupied = 0;
}

void TOMB_MemoryArenaFromOffset(TOMB_MemoryArena * pArena, TOMB_MemoryArena * pOffsetArena, Uint64 size) 
{
	TOMB_MemoryArenaInitialize(pArena, (Uint8 *)(pOffsetArena->Address + pOffsetArena->Size), size);
}

void * TOMB_MemoryArenaReserve(TOMB_MemoryArena * pArena, Uint64 size)
{
	SDL_assert(pArena->Occupied + size <= pArena->Size);

	void * memory = pArena->Address + pArena->Occupied;

	pArena->Occupied += size;

	return memory;
}

void TOMB_MemoryArenaFree(TOMB_MemoryArena * pArena, Uint64 size)
{
	SDL_assert(pArena->Occupied - size >= 0);

	pArena->Occupied -= size;
}