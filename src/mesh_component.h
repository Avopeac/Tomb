#pragma once

#include <vector>

#include "scene_component.h"

#include "data_pipe_hub.h"

namespace scene {

	class MeshComponent : public SceneComponent
	{
		std::string mesh_name_;

		std::string mesh_file_path_;

		size_t mesh_hash_;

		std::string texture_file_path_;

		size_t texture_hash_;

		glm::mat4 local_transform_;

	public:

		MeshComponent(const std::string &mesh_name, const std::string &mesh_file_path, const std::string &texture_file_path);

		~MeshComponent();

		virtual void Initialize() override;

		virtual void Update(float delta_time) override;

		inline const glm::mat4 &GetLocalTransform() const { return local_transform_; }

		inline void SetLocalTransform(const glm::mat4 &transform) { local_transform_ = transform; }
	};
}
