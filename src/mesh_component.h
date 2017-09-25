#pragma once

#include <vector>

#include "scene_component.h"

#include "mesh.h"

namespace scene {

	struct Mesh
	{
		size_t mesh;
		glm::mat4 transform;
	};

	class MeshComponent : public SceneComponent
	{
		
		std::vector<Mesh> drawables_;

	public:

		MeshComponent() {};

		~MeshComponent() {};

		void Push(Mesh && drawable);

		// Inherited via SceneComponent
		virtual void Initialize() override;

		virtual void Update(float delta_time) override;

	};
}
