#pragma once

#include "system.h"

#include "mesh_component.h"

namespace entity 
{

	class MeshRenderSystem : public System<MeshRenderSystem, MeshComponent>  
	{

	public:

		MeshRenderSystem() {}

		virtual ~MeshRenderSystem() {}
		
		// Inherited via System
		virtual void Initialize(Entity * entity) override;

		virtual void Update(Entity * entity) override;

		virtual void Clean(Entity * entity) override;

	};
}
