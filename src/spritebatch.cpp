#include "spritebatch.h"

#include <algorithm>

namespace graphics
{
	SpriteBatch::SpriteBatch()
	{
	}

	SpriteBatch::~SpriteBatch()
	{
	}

	void SpriteBatch::Begin()
	{
		sprites_.clear();
	}

	void SpriteBatch::Push(const Sprite &sprite)
	{
		sprites_.push_back(sprite);
	}

	void SpriteBatch::End()
	{
		std::sort(sprites_.begin(), sprites_.end(), [](const Sprite &a, const Sprite &b)
		{
			return (a.layer == b.layer) ? 
				(a.texture_hash < b.texture_hash) :
				(a.layer < b.layer);
		});
	}
}
