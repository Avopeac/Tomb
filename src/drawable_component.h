#pragma once

#include <vector>

#include "scene_component.h"

#include "mesh.h"

namespace scene {

	struct Drawable
	{
		bool flush;
		size_t mesh;
		glm::mat4 transform;
	};

	class DrawableComponent : public SceneComponent
	{
		
		std::vector<Drawable> transient_drawables_;

		std::vector<Drawable> constant_drawables_;

	public:

		DrawableComponent() {};

		~DrawableComponent() {};

		void Push(Drawable && drawable);

		// Inherited via SceneComponent
		virtual void Initialize() override;

		virtual void Update(float delta_time) override;

	};
}
