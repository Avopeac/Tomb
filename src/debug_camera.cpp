#include "debug_camera.h"
#include "keymap.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

using namespace graphics;

DebugCamera::DebugCamera(const GraphicsBase &graphics_base, const std::string & name, 
	const glm::vec4 &position,
	const glm::vec4 &forward,
	const glm::vec4 &up) :
	AbstractCamera(name, position, forward, up),
	graphics_base_(graphics_base),
	initial_position(position),
	initial_forward_(forward),
	initial_up_(up)
{
}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Update(float delta_time)
{
	using namespace input;
	auto &keymap = Keymap::Get();

	float look_speed = 15.0f;
	if (keymap.KeyPressed(Key::KeyUp)) { euler_angles_.y += look_speed * delta_time / graphics_base_.GetBackbufferHeight(); }
	if (keymap.KeyPressed(Key::KeyDown)) { euler_angles_.y -= look_speed * delta_time / graphics_base_.GetBackbufferHeight(); }
	if (keymap.KeyPressed(Key::KeyLeft)) { euler_angles_.x -= look_speed * delta_time  / graphics_base_.GetBackbufferWidth(); }
	if (keymap.KeyPressed(Key::KeyRight)) { euler_angles_.x += look_speed * delta_time / graphics_base_.GetBackbufferWidth(); }

	glm::mat4 rotation = glm::eulerAngleYXZ(
		glm::two_pi<float>() - glm::radians(1000.0f * euler_angles_.x),
		glm::two_pi<float>() - glm::radians(1000.0f * euler_angles_.y),
		glm::radians(euler_angles_.z));
	 
	glm::vec4 velocity{};
	glm::vec4 initial_right = glm::normalize(glm::vec4(glm::cross(glm::vec3(initial_forward_), 
		glm::vec3(initial_up_)), 0)); 
	 
	if (keymap.KeyPressed(Key::KeyW)) { velocity += initial_forward_; }
	if (keymap.KeyPressed(Key::KeyS)) {	velocity -= initial_forward_; }
	if (keymap.KeyPressed(Key::KeyA)) {	velocity -= initial_right; }
	if (keymap.KeyPressed(Key::KeyD)) {	velocity += initial_right; }
	if (keymap.KeyPressed(Key::KeyQ)) { velocity += initial_up_; }
	if (keymap.KeyPressed(Key::KeyE)) { velocity -= initial_up_; }

	forward_ = rotation * initial_forward_;
	up_ = rotation * initial_up_;

	if (glm::length(velocity) > 0.001f)
	{
		position_ = position_ + rotation * 10.0f * glm::normalize(velocity) * delta_time;
	}

	view_ = glm::lookAt(glm::vec3(position_), glm::vec3(position_) + glm::vec3(forward_), glm::vec3(up_));
	inv_view_ = glm::inverse(view_);
	proj_ = glm::perspective(glm::radians(60.0f), (float)graphics_base_.GetBackbufferWidth() / graphics_base_.GetBackbufferHeight(), 0.01f, 100.0f);
	inv_proj_ = glm::inverse(proj_);
	viewproj_ = proj_ * view_;
	inv_viewproj_ = glm::inverse(viewproj_);
}

void DebugCamera::Render()
{
}
