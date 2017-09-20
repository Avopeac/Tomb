#include "shadow_camera.h"

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
	proj_ = glm::ortho(0.0f, width_, 0.0f, height_, near_, far_);
	view_ = glm::lookAt(glm::vec3(position_), glm::vec3(forward_), glm::vec3(up_));
	view_proj_ = proj_ * view_;
	inv_proj_ = glm::inverse(proj_);
	inv_view_ = glm::inverse(view_);
	inv_view_proj_ = glm::inverse(view_proj_);
}
