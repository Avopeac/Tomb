#pragma once

#include <unordered_map>
#include <string>
#include "graphics.h"
#include "resource_manager.h"

namespace graphics
{

	struct MeshRenderInstance
	{
		Texture * texture;
		Mesh * mesh;
		glm::mat4 model;
		glm::vec4 color;
		std::function<void(Program * vertex, Program * fragment)> update;

		bool operator<(const MeshRenderInstance &other) const
		{
			if (!other.mesh || !other.texture)
				return false;

			return mesh->vao < other.mesh->vao;
		}
	};

	class MeshRenderer
	{
		const size_t max_instances_ = 2048;

		ProgramPipeline pipeline_;

		GraphicsBase & graphics_base_;

		std::vector<MeshRenderInstance> meshes_;

	public:

		MeshRenderer(GraphicsBase & graphics_base);

		~MeshRenderer();

		void Push(size_t mesh_hash, size_t texture_hash, const glm::mat4 &model, const std::function<void(Program *, Program *)> &update);

		void Draw(float delta_time);

	};
}