#pragma once

#include <vector>

#include "SDL.h"

#include "glm/glm.hpp"

namespace graphics
{
	struct Sprite
	{
		Uint32 type;
		Uint32 layer;
		size_t texture_hash;
		glm::ivec2 position;
	};

	class SpriteBatch
	{

		std::vector<Sprite> sprites_;

	public:

		SpriteBatch();

		~SpriteBatch();

		void Begin();

		void Push(const Sprite &sprite);

		void End();
	};

}
