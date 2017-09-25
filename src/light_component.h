#pragma once

#include <vector>

#include "scene_component.h"

#include "mesh.h"

namespace scene {

	struct Light
	{
		bool shadow_casting;
		bool directional;
		glm::vec3 direction;
		glm::vec3 position;
	};

	class LightComponent : public SceneComponent
	{

	public:

		LightComponent() {};

		~LightComponent() {};

		// Inherited via SceneComponent
		virtual void Initialize() override;

		virtual void Update(float delta_time) override;

	};
}
