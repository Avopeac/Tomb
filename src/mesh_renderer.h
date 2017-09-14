#pragma once

#include <unordered_map>
#include <string>

#include "graphics.h"
#include "blend_mode.h"
#include "shader.h"
#include "frame_buffer.h"
#include "sampler.h"
#include "texture.h"
#include "mesh.h"

#include "debug_camera.h"

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

		Program vertex_program_;

		Program fragment_program_;

		ProgramPipeline pipeline_;

		ProgramCache & program_cache_;

		GraphicsBase & graphics_base_;

		TextureCache & texture_cache_;

		SamplerCache & sampler_cache_;

		BlendCache & blend_cache_;

		MeshCache & mesh_cache_;

		std::vector<MeshRenderInstance> meshes_;

		DebugCamera camera_;

	public:

		MeshRenderer(GraphicsBase & graphics_base, ProgramCache & program_cache, TextureCache & texture_cache, 
			SamplerCache &sampler_cache, BlendCache &blend_cache, MeshCache &mesh_cache);

		~MeshRenderer();

		void Push(size_t mesh_hash, size_t texture_hash, const glm::mat4 &model, const std::function<void(Program *, Program *)> &update);

		void Draw(float delta_time);

		DebugCamera &GetCamera() { return camera_; }
	};
}