#pragma once

#include "camera_node.h"

namespace scene
{
	class DebugCameraNode : public CameraNode
	{
		glm::vec3 euler_angles_;
		glm::vec4 initial_position;
		glm::vec4 initial_forward_;
		glm::vec4 initial_up_;
		float aspect_ratio_;
		float fov_;
		float near_, far_;

	public:

		DebugCameraNode(float aspect_ratio, float fov,
			float near, float far,
			const glm::vec4 &position,
			const glm::vec4 &forward,
			const glm::vec4 &up);

		virtual ~DebugCameraNode();

		virtual void Update(float delta_time);

	};
}
