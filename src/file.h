#pragma once

#include <string>

#include "SDL.h"

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
