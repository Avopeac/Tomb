#pragma once

#include "abstract_camera.h"

#include "graphics.h"

namespace graphics
{
	class DebugCamera : public AbstractCamera
	{

		const GraphicsBase &graphics_base_;

		glm::vec3 euler_angles_;
		glm::vec4 initial_position;
		glm::vec4 initial_forward_;
		glm::vec4 initial_up_;

	public:

		DebugCamera(const GraphicsBase &graphics_base, const std::string &name, 
			const glm::vec4 &position,
			const glm::vec4 &direction,
			const glm::vec4 &up);

		~DebugCamera();

		virtual void Update(float delta_time) override;

		virtual void Render() override;

	};
}
