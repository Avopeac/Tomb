#pragma once

#include <string>
#include <iostream>

#include "SDL.h"

namespace debug
{

	static void Log(SDL_LogPriority priority, Sint32 category, const char * message)
	{
		std::string priority_string;

		switch (priority)
		{
		case SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL: { priority_string = "CRITICAL"; } break;
		case SDL_LogPriority::SDL_LOG_PRIORITY_INFO: { priority_string = "INFO"; } break;
		case SDL_LogPriority::SDL_LOG_PRIORITY_WARN: { priority_string = "WARNING"; } break;
		case SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE: { priority_string = "VERBOSE"; } break;
		case SDL_LogPriority::SDL_LOG_PRIORITY_DEBUG: { priority_string = "DEBUG"; } break;
		case SDL_LogPriority::SDL_LOG_PRIORITY_ERROR: { priority_string = "ERROR"; } break;
		default: { priority_string = "UNDEFINED"; } break;
		}

		std::string category_string;

		switch (category)
		{
		case SDL_LOG_CATEGORY_VIDEO: { category_string = "VIDEO"; } break;
		case SDL_LOG_CATEGORY_ASSERT: { category_string = "ASSERT"; } break;
		case SDL_LOG_CATEGORY_AUDIO: { category_string = "AUDIO"; } break;
		case SDL_LOG_CATEGORY_ERROR: { category_string = "ERROR"; } break;
		case SDL_LOG_CATEGORY_INPUT: { category_string = "INPUT"; } break;
		case SDL_LOG_CATEGORY_RENDER: { category_string = "RENDER"; } break;
		case SDL_LOG_CATEGORY_TEST: { category_string = "TEST"; } break;
		case SDL_LOG_CATEGORY_SYSTEM: { category_string = "SYSTEM"; } break;
		case SDL_LOG_CATEGORY_APPLICATION: { category_string = "APPLICATION"; } break;
		default: { category_string = "UNDEFINED"; } break;
		}

		std::cout << "LOG-" << priority_string << "-" << category_string << ":" << message << std::endl;
	}
}

