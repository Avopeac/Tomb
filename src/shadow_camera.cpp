#include "shadow_camera.h"

#include "keymap.h"

#include "glm/gtc/matrix_transform.hpp"

using namespace graphics;

ShadowCamera::ShadowCamera(const std::string &name,
	float width, float height,
	float near, float far,
	const glm::vec4 &position,
	const glm::vec4 &direction,
	const glm::vec4 &up) :
	AbstractCamera(name, position, direction, up),
	width_(width),
	height_(height),
	near_(near),
	far_(far)
{

}

ShadowCamera::~ShadowCamera()
{
}

void ShadowCamera::Update(float delta_time)
{
	static glm::vec3 pos = position_;

	using namespace input;
	auto &keymap = Keymap::Get();

	if (keymap.KeyPressed(Key::KeyI)) { position_.z += 1.0f * delta_time; }
	if (keymap.KeyPressed(Key::KeyK)) { position_.z -= 1.0f * delta_time; }
	if (keymap.KeyPressed(Key::KeyJ)) { position_.x -= 1.0f * delta_time; }
	if (keymap.KeyPressed(Key::KeyL)) { position_.x += 1.0f * delta_time; }
	if (keymap.KeyPressed(Key::KeyU)) { position_.y += 1.0f * delta_time; }
	if (keymap.KeyPressed(Key::KeyO)) { position_.y -= 1.0f * delta_time; }



	proj_ = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
	//proj_ = glm::perspective(glm::radians(60.0f), width_ / height_, 0.01f, 100.0f);
	//view_ = glm::lookAt(glm::vec3(position_), glm::vec3(position_ + forward_), glm::vec3(up_));
	view_ = glm::lookAt(glm::vec3(position_), glm::vec3(forward_), glm::vec3(up_));
	view_proj_ = proj_ * view_;
	inv_proj_ = glm::inverse(proj_);
	inv_view_ = glm::inverse(view_);
	inv_view_proj_ = glm::inverse(view_proj_);
}
