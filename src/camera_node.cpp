#include "camera_node.h"

using namespace scene;

scene::CameraNode::CameraNode(const std::string &name, 
	const glm::vec4 &position,
	const glm::vec4 &forward,
	const glm::vec4 &up) :
	SceneNode(name),
	position_(position),
	forward_(forward),
	up_(up)
{
}

scene::CameraNode::~CameraNode()
{
}

void scene::CameraNode::Update(float delta_time)
{
	SceneNode::Update(delta_time);
}
