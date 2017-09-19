#include "debug_camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "keymap.h"

using namespace graphics;

DebugCamera::DebugCamera(const std::string & name, float aspect_ratio, float fov, 
	float near, float far,
	const glm::vec4 &position,
	const glm::vec4 &forward,
	const glm::vec4 &up) :
	AbstractCamera(name, position, forward, up),
	aspect_ratio_(aspect_ratio),
	fov_(fov),
	near_(near),
	far_(far),
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

	float look_speed = 0.001f;
	if (keymap.KeyPressed(Key::KeyUp)) { euler_angles_.y += look_speed * delta_time; }
	if (keymap.KeyPressed(Key::KeyDown)) { euler_angles_.y -= look_speed * delta_time; }
	if (keymap.KeyPressed(Key::KeyLeft)) { euler_angles_.x -= look_speed * delta_time; }
	if (keymap.KeyPressed(Key::KeyRight)) { euler_angles_.x += look_speed * delta_time; }

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
	proj_ = glm::perspective(fov_, aspect_ratio_, near_, far_);
	viewproj_ = proj_ * view_;
	inv_view_ = glm::inverse(view_);
	inv_proj_ = glm::inverse(proj_);
	inv_viewproj_ = glm::inverse(viewproj_);
}

void DebugCamera::Render()
{
}
