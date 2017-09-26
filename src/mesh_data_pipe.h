#pragma once

#include <vector>

#include "glm/glm.hpp"

namespace graphics {

	struct MeshData
	{
		size_t mesh_hash;
		size_t texture_hash;
		glm::mat4 world_transform;
	};

	class MeshDataPipe
	{

		std::vector<MeshData> data_;

	public:
		
		MeshDataPipe() {}

		~MeshDataPipe() {}

		void Push(const MeshData &mesh_data)
		{
			data_.push_back(mesh_data);
		}

		void Flush()
		{
			data_.clear();
		}

		inline const std::vector<MeshData> &GetData() const { return data_; }
	};
}
