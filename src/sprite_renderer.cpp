#include "sprite_renderer.h"

using namespace graphics;

SpriteRenderer::SpriteRenderer()
{
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Push(const Sprite & sprite)
{
	sprites_.push_back(sprite);
}

void SpriteRenderer::Process()
{
	for (const auto &sprite : sprites_)
	{
		unique_textures_.insert(sprite.GetTexture());
		unique_animations_.insert(sprite.GetAnimation());	
	}

	size_t num_unique_textures = unique_textures_.size();
	size_t num_unique_animations = unique_animations_.size();



	sprites_.clear();
}

void SpriteRenderer::Draw()
{
}
