#pragma once

#include "SDL.h"

namespace util
{
	static double GetSeconds()
	{
		static Uint64 frequency = SDL_GetPerformanceFrequency();
		static double start = (double)SDL_GetPerformanceCounter() / frequency;
		return (double)SDL_GetPerformanceCounter() / frequency - start;
	}
}