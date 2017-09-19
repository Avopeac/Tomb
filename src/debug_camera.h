#pragma once

#include "abstract_camera.h"

namespace graphics
{
	class DebugCamera : public AbstractCamera
	{
		glm::vec3 euler_angles_;
		glm::vec4 initial_position;
		glm::vec4 initial_forward_;
		glm::vec4 initial_up_;
		float aspect_ratio_;
		float fov_;
		float near_, far_;

	public:

		DebugCamera(const std::string &name, float aspect_ratio, float fov,
			float near, float far,
			const glm::vec4 &position,
			const glm::vec4 &direction,
			const glm::vec4 &up);

		~DebugCamera();

		virtual void Update(float delta_time) override;

	};
}
