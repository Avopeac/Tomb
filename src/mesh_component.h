#pragma once

#include <glm/glm.hpp>

#include "component.h"

namespace entity 
{
	class MeshComponent : public Component
	{

		friend class MeshRenderSystem;

		size_t mesh_hash_;
		size_t texture_hash_;

		std::string mesh_name_, mesh_path_;
		std::string texture_path_;
		glm::mat4 transform_;

		public:

			MeshComponent(Entity * entity, const std::string &mesh_name,
				const std::string &mesh_path,
				const std::string &texture_path) : Component(entity),
				mesh_name_(mesh_name),
				mesh_path_(mesh_path),
				texture_path_(texture_path) {}

			virtual ~MeshComponent() {}

			inline void SetTransform(const glm::mat4 &transform) { transform_ = transform; }

			inline void SetTexture(const std::string &texture_path) { texture_path_ = texture_path; }

			inline const glm::mat4 &GetTransform() const { return transform_; }

			inline const std::string &GetMeshName() const { return mesh_name_; }

			inline const std::string &GetMeshPath() const { return mesh_path_; }

			inline const std::string &GetTexturePath() const { return texture_path_; }
	};
}
