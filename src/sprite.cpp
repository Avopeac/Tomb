#include "Sprite.h"

using namespace graphics;

Sprite::Sprite(glm::mat4 transform, size_t layer) :
	transform_(transform), layer_(layer)
{
}

Sprite::Sprite(const Sprite &other) :
	transform_(other.transform_), 
	layer_(other.layer_)
{
}

Sprite::Sprite(Sprite &&other) :
	transform_(other.transform_),
	layer_(other.layer_)
{
}

Sprite::~Sprite()
{
}
