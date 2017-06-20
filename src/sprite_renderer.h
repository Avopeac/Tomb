#pragma once

#include <vector>

#include <set>

#include "sprite.h"

namespace graphics
{

	class SpriteRenderer
	{

		std::vector<Sprite> sprites_;

		std::set<size_t> unique_textures_;

		std::set<size_t> unique_animations_;

		std::vector<std::vector<Sprite>> batches_;

	public:

		~SpriteRenderer();

		SpriteRenderer(const SpriteRenderer &) = delete;

		SpriteRenderer(const SpriteRenderer &&) = delete;

		SpriteRenderer &operator=(const SpriteRenderer &) = delete;

		SpriteRenderer &operator=(SpriteRenderer &&) = delete;

		static SpriteRenderer &Get()
		{
			static SpriteRenderer instance;
			return instance;
		}

		void Push(const Sprite &sprite);

		void Process();

		void Draw();

	private:

		SpriteRenderer();

	};
}
