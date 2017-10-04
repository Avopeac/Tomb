#pragma once

#include <unordered_map>
#include <string>
#include "graphics.h"
#include "resource_manager.h"

namespace graphics
{
	struct MeshRenderInstance
	{
		Mesh * mesh;
		Texture * texture;
		GLuint world_transform_buffer;
		std::vector<glm::mat4> world_transforms;

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

		Program * geometry_vertex_;
		Program * geometry_fragment_;

		Program * shadow_vertex_;
		Program * shadow_fragment_;

		FrameBuffer * shadow_map_;
		FrameBuffer * gbuffer_;

		AbstractCamera * shadow_camera_;
		AbstractCamera * main_camera_;

		GraphicsBase & graphics_base_;

		std::unordered_map<size_t, MeshRenderInstance> meshes_;

	public:

		MeshRenderer(GraphicsBase & graphics_base);

		~MeshRenderer();

		void Draw(float delta_time);

	private: 

		void RenderShadows();

		void RenderGeometry();

		void PreProcessGeometry();

	};
}