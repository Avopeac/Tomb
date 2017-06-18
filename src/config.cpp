#include "config.h"

#include "json.h"

using namespace input;

Config::Config()
{
}

Config::~Config()
{
}

void Config::Load(const std::string & path)
{

	JsonReader reader;
	reader.ReadJsonFile(path);
	while (reader.NextToken())
	{
		if (reader.GetTokenType() == JsonTokenType::String)
		{
			std::string value = reader.GetString();

			if (value == "vsync")
			{
				reader.NextToken();
				vertical_sync_ = (Sint8)reader.GetNumber();
			}
			else if (value == "width")
			{
				reader.NextToken();
				window_width_ = (Uint16)reader.GetNumber();
			}
			else if (value == "height")
			{
				reader.NextToken();
				window_height_ = (Uint16)reader.GetNumber();
			}
			else if (value == "title")
			{
				reader.NextToken();
				window_title_ = reader.GetString();
			}
		}
	}
}
