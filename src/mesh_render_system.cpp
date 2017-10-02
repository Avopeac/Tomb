#include "mesh_render_system.h"

#include "entity_manager.h"
#include "resource_manager.h"
#include "data_pipe_hub.h"

using namespace entity;

void MeshRenderSystem::Initialize(Entity * entity)
{

	if (!entity)
	{
		return;
	}

	auto * component = EntityManager::Get().GetEntityComponent<MeshComponent>(entity->id);
	if (component)
	{
		auto &mesh_cache = graphics::ResourceManager::Get().GetMeshCache();
		auto &texture_cache = graphics::ResourceManager::Get().GetTextureCache();
		mesh_cache.GetFromFile(component->GetMeshName(), component->GetMeshPath(), &component->mesh_hash_);
		texture_cache.GetFromFile(component->GetTexturePath(), true, &component->texture_hash_);
	}
	
}

void MeshRenderSystem::Update(Entity * entity)
{
	if (!entity)
	{
		return;
	}
	
	auto * component = EntityManager::Get().GetEntityComponent<MeshComponent>(entity->id);
	if (component)
	{
		auto &mesh_data_pipe = graphics::DataPipeHub::Get().GetMeshDataPipe();

		graphics::MeshData mesh_data;
		mesh_data.mesh_hash = component->mesh_hash_;
		mesh_data.texture_hash = component->texture_hash_;
		mesh_data.world_transform = component->transform_;

		// Push data
		mesh_data_pipe.Push(mesh_data);
	}
}

void MeshRenderSystem::Clean(Entity * entity)
{
	// TODO: Implement
}
