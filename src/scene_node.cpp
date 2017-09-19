#include "scene_node.h"

graphics::SceneNode::SceneNode()
{
}

graphics::SceneNode::~SceneNode()
{
}

bool graphics::SceneNode::RemoveChildren()
{
	return false;
}

void graphics::SceneNode::RemoveChild(SceneNode * child_node)
{
}

void graphics::SceneNode::RemoveChild(size_t child_id)
{
}

void graphics::SceneNode::AddChild(SceneNode * child_node)
{
}

void graphics::SceneNode::Update(float delta_time)
{
}
