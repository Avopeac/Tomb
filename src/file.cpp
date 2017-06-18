#include "file.h"

#include <fstream>
#include <sstream>

using namespace input;

FileReader::FileReader()
{
}

FileReader::~FileReader()
{
}

std::string FileReader::ReadTextFile(const std::string & path)
{
	std::ostringstream output_stream;

	std::ifstream input_stream(path, std::ifstream::in);

	if (!input_stream.is_open())
	{
		SDL_assert(false);
	}

	output_stream << input_stream.rdbuf();

	return output_stream.str();
}
