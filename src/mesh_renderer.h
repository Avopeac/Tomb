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

namespace graphics
{

	struct MeshRenderInstance
	{
		Texture * texture;
		glm::mat4 model;
		glm::vec4 color;
		Mesh * mesh;
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

	public:

		MeshRenderer(GraphicsBase & graphics_base, ProgramCache & program_cache, TextureCache & texture_cache, 
			SamplerCache &sampler_cache, BlendCache &blend_cache, MeshCache &mesh_cache);

		~MeshRenderer();

		void Push(size_t mesh_hash, size_t texture_hash, const glm::mat4 &model, const glm::vec4 &color);

		void Draw();
	};
}