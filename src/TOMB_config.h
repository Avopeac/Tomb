#pragma once

#include "SDL.h"

#include "TOMB_defines.h"

struct TOMB_Config
{
	Sint8 verticalSync;
	Uint16 windowWidth;
	Uint16 windowHeight;
	const char * pWindowTitle;
};

Sint32 TOMB_ConfigLoadFile(TOMB_Config * pConfig, const char * pConfigPath) {

	pConfig->verticalSync = 1;
	pConfig->windowWidth = 512;
	pConfig->windowHeight = 512;
	pConfig->pWindowTitle = "Tomb";

	return TOMB_SUCCESS;
}
