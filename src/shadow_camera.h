#pragma once

#include "abstract_camera.h"

namespace graphics {

	class ShadowCamera : public AbstractCamera
	{
		float width_, height_;
		float near_, far_;

	public:

		ShadowCamera(const std::string &name, 
			float width, float height,
			float near, float far,
			const glm::vec4 &position,
			const glm::vec4 &direction,
			const glm::vec4 &up);

		~ShadowCamera();

		inline void SetPosition(const glm::vec4 &position) { position_ = position; }

		inline void SetDirection(const glm::vec4 &direction) { forward_ = direction; }

		inline void SetUp(const glm::vec4 &up) { up_ = up; }

		virtual void Update(float delta_time) override;
	};
}
