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

	bool relative_mouse = SDL_GetRelativeMouseMode() == 0 ? true : false;

	if (relative_mouse)
	{
		int x, y;
		float mouse_speed = 15.0f;
		SDL_GetRelativeMouseState(&x, &y);
		euler_angles_.x += mouse_speed * delta_time * (float)x / graphics_base_.GetBackbufferWidth();
		euler_angles_.y += mouse_speed * delta_time * (float)y / graphics_base_.GetBackbufferHeight();
		euler_angles_.z += 0.0f;
	}

	std::cout << euler_angles_.x << " " << euler_angles_.y << std::endl;

	if (keymap.KeyDown(Key::KeyTab))
	{
		SDL_bool is_relative = relative_mouse ? SDL_TRUE : SDL_FALSE;
		int show_cursor = relative_mouse ? 0 : 1;
		SDL_SetRelativeMouseMode(is_relative);
		SDL_ShowCursor(show_cursor);
	}
	
	glm::mat4 rotation = glm::eulerAngleYXZ(
		glm::radians(360.0f * euler_angles_.x),
		glm::radians(euler_angles_.y),
		glm::radians(euler_angles_.z));

	glm::vec4 velocity{};
	glm::vec4 initial_right = glm::normalize(glm::vec4(glm::cross(glm::vec3(initial_forward_), 
		glm::vec3(initial_up_)), 0));

	if (keymap.KeyPressed(Key::KeyW)) { velocity += initial_forward_; }
	if (keymap.KeyPressed(Key::KeyS)) {	velocity -= initial_forward_; }
	if (keymap.KeyPressed(Key::KeyA)) {	velocity -= initial_right; }
	if (keymap.KeyPressed(Key::KeyD)) {	velocity += initial_right; }

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
