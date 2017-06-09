#pragma once

//#include "jsmn/jsmn.h"

#include "jsmn.h"

#include "SDL.h"

#include "TOMB_defines.h"
#include "TOMB_file.h"
#include "TOMB_memory.h"

struct TOMB_Config
{
	Sint8 verticalSync;
	Uint16 windowWidth;
	Uint16 windowHeight;
	const char * pWindowTitle;
};

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)SDL_strlen(s) == tok->end - tok->start &&
		SDL_strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

Sint32 TOMB_ConfigLoadFile(TOMB_MemoryArena * pArena, TOMB_Config * pConfig, const char * pConfigPath) {

	Sint64 size;
	const char * pContent = TOMB_ReadTextFile(pArena, pConfigPath, &size);

	jsmn_parser jsonParser;
	jsmn_init(&jsonParser);
	jsmntok_t jsonTokens[128];
	int status = jsmn_parse(&jsonParser, pContent, size, jsonTokens, sizeof(jsonTokens) / sizeof(jsonTokens[0]));
	if (status < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Failed to parse JSON file %s.", pConfigPath);
		return TOMB_FAILURE;
	}

	for (int i = 1; i < status; ++i) {

		if (jsoneq(pContent, &jsonTokens[i], "vsync") == 0) {
			pConfig->verticalSync = SDL_strtol(pContent + jsonTokens[i + 1].start, nullptr, 10);
			++i;
		}
		else if (jsoneq(pContent, &jsonTokens[i], "width") == 0) {
			pConfig->windowWidth = SDL_strtol(pContent + jsonTokens[i + 1].start, nullptr, 10);
			++i;
		}
		else if (jsoneq(pContent, &jsonTokens[i], "height") == 0) {
			pConfig->windowHeight = SDL_strtol(pContent + jsonTokens[i + 1].start, nullptr, 10);
			++i;
		}
		else if (jsoneq(pContent, &jsonTokens[i], "title") == 0) {
			pConfig->pWindowTitle = SDL_strdup(pContent + jsonTokens[i + 1].end - jsonTokens[i + 1].start);
			++i;
		}
	}

	return TOMB_SUCCESS;
}
