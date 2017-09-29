#pragma once

#include "system.h"

#include "mesh_component.h"

namespace entity 
{
	class Entity;

	class MeshRenderSystem : public System<MeshRenderSystem, MeshComponent>  
	{

	public:

		MeshRenderSystem() {}

		virtual ~MeshRenderSystem() {}

		void Initialize(Entity * entity);

		void Update(Entity * entity);

		void Clean(Entity * entity);
	};
}
