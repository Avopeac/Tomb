#include "mesh_component.h"

#include "scene_object.h"

using namespace scene;

#include "resource_manager.h"

MeshComponent::MeshComponent(const std::string & mesh_name, const std::string & mesh_file_path, const std::string & texture_file_path) :
	mesh_name_(mesh_name), mesh_file_path_(mesh_file_path), texture_file_path_(texture_file_path)
{
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::Initialize()
{
	auto &mesh_cache = graphics::ResourceManager::Get().GetMeshCache();
	mesh_cache.GetFromFile(mesh_name_, mesh_file_path_, &mesh_hash_);

	auto &texture_cache = graphics::ResourceManager::Get().GetTextureCache();
	texture_cache.GetFromFile(texture_file_path_, true, &texture_hash_);
}

void MeshComponent::Update(float delta_time)
{
	graphics::MeshData data;
	data.mesh_hash = mesh_hash_;
	data.texture_hash = texture_hash_;
	data.world_transform = object_->GetTransform() * local_transform_;

	auto &hub = graphics::DataPipeHub::Get().GetMeshDataPipe();
	hub.Push(data);
}
