#pragma once

#include "graphics.h"
#include "texture.h"
#include "shader.h"
#include "frame_buffer.h"
#include "blend_mode.h"
#include "sampler.h"

namespace graphics
{
	class Cube
	{
		const size_t num_indices_ = 36;
		const size_t num_vertices_ = 8;

		const GraphicsBase &graphics_base_;

		TextureCache &texture_cache_;
		SamplerCache &sampler_cache_;
		FrameBufferCache &frame_buffer_cache_;
		ProgramCache &program_cache_;
		BlendCache &blend_cache_;

		GLuint vao_, vbo_, ebo_;

		const Program * vertex_shader_;
		const Program * fragment_shader_;

		ProgramPipeline pipeline_;

		glm::mat4 view_;
		glm::mat4 proj_;

	public:

		Cube(const GraphicsBase &graphics_base,
			TextureCache &texture_cache,
			SamplerCache &sampler_cache,
			FrameBufferCache &frame_buffer_cache,
			ProgramCache &program_cache,
			BlendCache &blend_cache);

		~Cube();

		void Init();

		void Render(float frame_time);

	};
}
