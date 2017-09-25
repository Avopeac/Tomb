#include "drawable_component.h"

#include "scene_object.h"

using namespace scene;

void DrawableComponent::Push(Drawable && drawable)
{
	drawable.transform = object_->GetTransform() * drawable.transform;

	if (drawable.flush)
	{
		transient_drawables_.push_back(drawable);
	}
	else
	{
		constant_drawables_.push_back(drawable);
	}
}

void DrawableComponent::Initialize()
{
}

void DrawableComponent::Update(float delta_time)
{
}
