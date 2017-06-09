#pragma once

#include "SDL.h"

#include "TOMB_memory.h"

const char * TOMB_ReadTextFile(TOMB_MemoryArena * pArena, const char * pPath, Sint64 * size)
{
	const char * mode = "rb";

	SDL_RWops * context = SDL_RWFromFile(pPath, mode);

	*size = SDL_RWsize(context);

	if (!context || *size == 0) 
	{
		SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Could not read from text file from path %s.", pPath);
		return nullptr;
	}

	char * content = (char *)TOMB_MemoryArenaReserve(pArena, (Uint64)*size + 1);

	Sint64 total = context->read(context, (void *)content, (size_t)*size, (size_t)*size);

	if (total != *size)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_INPUT, "Could not read the entire text file from path %s.", pPath);
	}

	content[*size] = '\0';

	return (const char *)content;
}
