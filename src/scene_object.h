#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "scene_component.h"

namespace scene {
	
	class SceneObject
	{
		glm::mat4 transform_;

		std::vector<SceneComponent> components_;

	public:

		SceneObject() {};

		~SceneObject() {};

		inline void SetTransform(const glm::mat4 &transform) { transform_ = transform;	}

		inline const glm::mat4 &GetTransform() const { return transform_; }

	};
}
