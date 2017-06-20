#include "Sprite.h"

using namespace graphics;

Sprite::Sprite(glm::mat4 transform) :
	transform_(transform)
{
}

Sprite::Sprite(const Sprite &other) :
	transform_(other.transform_)
{
	layer_ = other.layer_;
	texture_hash_ = other.texture_hash_;
	animation_hash_ = other.animation_hash_;
}

Sprite::Sprite(Sprite &&other) :
	transform_(other.transform_)
{
	layer_ = other.layer_;
	texture_hash_ = other.texture_hash_;
	animation_hash_ = other.animation_hash_;
}

Sprite::~Sprite()
{
}
