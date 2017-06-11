#pragma once

#include "SDL.h"

#include <cstring>

#include "jsmn/jsmn.h"

#include "TOMB_memory.h"

const char * TOMB_TextReadFile(TOMB_MemoryArena * pArena, const char * pPath, Sint64 * size)
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

struct TOMB_Json
{
	const char * pJsonRaw;
	jsmntok_t * pJsonTokens;
	size_t jsonTokenCount;
};

bool TOMB_JsonTokenEquals(TOMB_Json * json, int tokenIndex, const char * value)
{
	jsmntok_t * token = &json->pJsonTokens[tokenIndex];

	if (token->type == JSMN_STRING && (int)SDL_strlen(value) == token->end - token->start &&
		SDL_strncmp(json->pJsonRaw + token->start, value, token->end - token->start) == 0)
	{
		return true;
	}

	return false;
}

void TOMB_JsonGetStringToken(TOMB_Json * json, int tokenIndex, char * destination, Uint64 maxLength)
{
	SDL_assert(maxLength >= (json->pJsonTokens[tokenIndex].end - json->pJsonTokens[tokenIndex].start));
	SDL_strlcpy(destination, json->pJsonRaw + json->pJsonTokens[tokenIndex].start,
		json->pJsonTokens[tokenIndex].end - json->pJsonTokens[tokenIndex].start + 1);
}

void TOMB_JsonGetNumberToken(TOMB_Json * json, int tokenIndex, double * destination)
{
	*destination = SDL_strtod(json->pJsonRaw + json->pJsonTokens[tokenIndex].start, nullptr);
}

TOMB_Json TOMB_JsonReadFile(TOMB_MemoryArena * pArena, const char * pPath)
{
	jsmn_parser jsonParser;
	jsmn_init(&jsonParser);

	// Read the raw data
	TOMB_Json json;
	Sint64 rawSize;
	json.pJsonRaw = TOMB_TextReadFile(pArena, pPath, &rawSize);

	// Get number of JSON tokens
	int numTokens = jsmn_parse(&jsonParser, json.pJsonRaw, rawSize, nullptr, 0);
	if (numTokens < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Failed to parse JSON file %s.", pPath);
	}

	// Reset token position
	jsmn_init(&jsonParser);

	if (numTokens >= 1)
	{
		json.jsonTokenCount = numTokens;
		json.pJsonTokens = (jsmntok_t *)TOMB_MemoryArenaReserve(pArena, sizeof(jsmntok_t) * numTokens);
		jsmn_parse(&jsonParser, json.pJsonRaw, (size_t)rawSize, json.pJsonTokens, (Uint32)json.jsonTokenCount);
	}

	return json;
}
