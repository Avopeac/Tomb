#pragma once

#include <string>

#include "SDL.h"

#include "jsmn/jsmn.h"

#include "memory.h"

namespace input
{
	class FileReader
	{
	public:

		FileReader();

		~FileReader();

		std::string ReadTextFile(const std::string &path);

	};
}
