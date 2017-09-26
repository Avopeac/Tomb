#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "scene_component.h"

namespace scene {

	class LightComponent : public SceneComponent
	{

		bool shadow_;

		glm::vec3 position_;

		glm::vec3 direction_;

		glm::vec3 color_;

	public:

		LightComponent(bool shadow);

		~LightComponent();

		virtual void Initialize() override;

		virtual void Update(float delta_time) override;

		inline void SetPosition(const glm::vec3 &position) { position_ = position; }

		inline void SetDirection(const glm::vec3 &direction) { direction_ = direction; }

		inline void SetColor(const glm::vec3 &color) { color_ = color; }

		inline const glm::vec3 &GetPosition() const { return position_; }

		inline const glm::vec3 &GetDirection() const { return direction_; }

		inline const glm::vec3 &GetColor() const { return color_; }

		inline bool IsShadowEnabled() const { return shadow_; }
	};
}
