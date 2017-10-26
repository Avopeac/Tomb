#pragma once

#include <string>

#include "SDL.h"

namespace input
{
	class Config
	{

		Sint8 vertical_sync_;

		Uint16 window_width_;

		Uint16 window_height_;

		std::string window_title_;
		
	public:

		Config();

		~Config();

		void Load(const std::string &path);

		inline Sint8 GetVerticalSync() const { return vertical_sync_; }

		inline Uint16 GetWindowWidth() const { return window_width_; }

		inline Uint16 GetWindowHeight() const { return window_height_; }

		inline const std::string &GetWindowTitle() const { return window_title_; }

	private:

		void SetVerticalSync(Sint8 vsync);

		void SetWindowWidth(Uint16 width);

		void SetWindowHeight(Uint16 height);

		void SetWindowTitle(const std::string &title);

	};
}
