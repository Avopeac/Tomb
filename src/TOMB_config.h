#pragma once

#include "jsmn/jsmn.h"

#include "SDL.h"

#include "TOMB_defines.h"
#include "TOMB_file.h"
#include "TOMB_memory.h"

#define TOMB_CONFIG_TITLE_MAX_LENGTH 128

struct TOMB_Config
{
	Sint8 verticalSync;
	Uint16 windowWidth;
	Uint16 windowHeight;
	char pWindowTitle[TOMB_CONFIG_TITLE_MAX_LENGTH];
};

Sint32 TOMB_ConfigLoadFile(TOMB_MemoryArena * pArena, TOMB_Config * pConfig, const char * pConfigPath) {


	TOMB_Json config = TOMB_JsonReadFile(pArena, pConfigPath);

	double jsonNumber;

	// First one is the JSON object which is not of interest
	for (int i = 1; i < config.jsonTokenCount; ++i)
	{
		if (TOMB_JsonTokenEquals(&config, i, "vsync"))
		{
			TOMB_JsonGetNumberToken(&config, i + 1, &jsonNumber);
			pConfig->verticalSync = (Sint8)jsonNumber;
			++i;
		}
		else if (TOMB_JsonTokenEquals(&config, i, "width"))
		{
			TOMB_JsonGetNumberToken(&config, i + 1, &jsonNumber);
			pConfig->windowWidth = (Uint16)jsonNumber;
			++i;
		}
		else if (TOMB_JsonTokenEquals(&config, i, "height"))
		{
			TOMB_JsonGetNumberToken(&config, i + 1, &jsonNumber);
			pConfig->windowHeight = (Uint16)jsonNumber;
			++i;
		}
		else if (TOMB_JsonTokenEquals(&config, i, "title"))
		{
			TOMB_JsonGetStringToken(&config, i + 1, pConfig->pWindowTitle, TOMB_CONFIG_TITLE_MAX_LENGTH);
			++i;
		}
	}

	return TOMB_SUCCESS;
}
