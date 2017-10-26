#include "config.h"

#include "chaiscript/chaiscript.hpp"

using namespace input;

Config::Config()
{
}

Config::~Config()
{
}

void Config::Load(const std::string & path)
{
	chaiscript::ChaiScript chai;
	chai.add(chaiscript::fun(&Config::SetVerticalSync, this), "SetVerticalSync");
	chai.add(chaiscript::fun(&Config::SetWindowWidth, this), "SetWindowWidth");
	chai.add(chaiscript::fun(&Config::SetWindowHeight, this), "SetWindowHeight");
	chai.add(chaiscript::fun(&Config::SetWindowTitle, this), "SetWindowTitle");

	chai.eval_file(path);
}

void Config::SetVerticalSync(Sint8 vsync)
{
	vertical_sync_ = vsync;
}

void Config::SetWindowWidth(Uint16 width)
{
	window_width_ = width;
}

void Config::SetWindowHeight(Uint16 height)
{
	window_height_ = height;
}

void Config::SetWindowTitle(const std::string & title)
{
	window_title_ = title;
}
