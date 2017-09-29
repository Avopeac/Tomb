#include "mesh_render_system.h"

#include "entity.h"

#include "resource_manager.h"

#include "data_pipe_hub.h"

using namespace entity;

void MeshRenderSystem::Initialize(Entity * entity)
{

	if (!entity)
	{
		return;
	}

	auto * mesh_component = entity->GetComponent<MeshComponent>();

	auto &mesh_cache = graphics::ResourceManager::Get().GetMeshCache();
	auto &texture_cache = graphics::ResourceManager::Get().GetTextureCache();

	mesh_cache.GetFromFile(mesh_component->GetMeshName(), mesh_component->GetMeshPath(), &mesh_component->mesh_hash_);
	texture_cache.GetFromFile(mesh_component->GetTexturePath(), true, &mesh_component->texture_hash_);
}

void MeshRenderSystem::Update(Entity * entity)
{

	if (!entity)
	{
		return;
	}

	auto * mesh_component = entity->GetComponent<MeshComponent>();
	auto &mesh_data_pipe = graphics::DataPipeHub::Get().GetMeshDataPipe();

	graphics::MeshData mesh_data;
	mesh_data.mesh_hash = mesh_component->mesh_hash_;
	mesh_data.texture_hash = mesh_component->texture_hash_;
	mesh_data.world_transform = mesh_component->transform_;

	mesh_data_pipe.Push(mesh_data);
}

void MeshRenderSystem::Clean(Entity * entity)
{
	// TODO: Implement
}
