#pragma once

#include "abstract_camera.h"

#include "halton.h"

namespace graphics
{
	class MainCamera : public AbstractCamera
	{
		glm::vec3 euler_angles_;
		glm::vec4 initial_position;
		glm::vec4 initial_forward_;
		glm::vec4 initial_up_;
		glm::vec2 window_size_;
		float aspect_ratio_;
		float fov_;
		float near_, far_;

	public:

		MainCamera(const std::string &name, float aspect_ratio, float fov,
			float near, float far,
			const glm::vec4 &position,
			const glm::vec4 &direction,
			const glm::vec4 &up);

		~MainCamera();

		virtual void Update(float delta_time) override;

	};
}
