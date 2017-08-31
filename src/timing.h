#pragma once

#include "SDL.h"

namespace util
{
	static double GetSeconds()
	{
		static Uint64 frequency = SDL_GetPerformanceFrequency();
		return ((double)SDL_GetPerformanceCounter() / frequency) / 1000.0;
	}
}