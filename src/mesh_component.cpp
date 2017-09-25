#include "mesh_component.h"

#include "scene_object.h"

using namespace scene;

void MeshComponent::Push(Mesh && drawable)
{
	drawable.transform = object_->GetTransform() * drawable.transform;
	drawables_.push_back(drawable);
}

void MeshComponent::Initialize()
{

}

void MeshComponent::Update(float delta_time)
{
}
