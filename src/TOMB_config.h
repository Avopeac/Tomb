#pragma once

#include "SDL.h"

#include "TOMB_defines.h"

struct TOMB_Config
{
	Sint8 VerticalSync;
	Uint16 WindowWidth;
	Uint16 WindowHeight;
	const char * WindowTitle;
};

Sint32 TOMB_ConfigLoadFile(TOMB_Config * config, const char * configPath) {

	config->VerticalSync = 1;
	config->WindowWidth = 512;
	config->WindowHeight = 512;
	config->WindowTitle = "Tomb";

	return TOMB_SUCCESS;
}
