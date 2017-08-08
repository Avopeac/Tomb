#pragma once

#include <functional>
#include <vector>
#include <algorithm>

#include "graphics.h"

#include "shader.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"
#include "frame_buffer.h"

namespace graphics
{

	class PostProcessEffect
	{
		static GLuint vao_, vbo_, ebo_;

	public:

		PostProcessEffect() = default;

		~PostProcessEffect() = default;

		virtual void Init(TextureCache &texture_cache,
			ProgramCache &program_cache,
			SamplerCache &sampler_cache,
			BlendCache &blend_cache,
			FrameBufferCache &frame_buffer_cache) = 0;
 
		virtual FrameBuffer * Apply(TextureCache &texture_cache,
			ProgramCache &program_cache,
			SamplerCache &sampler_cache,
			BlendCache &blend_cache,
			FrameBufferCache &frame_buffer_cache) = 0;
		
		static void Init(const GraphicsBase &graphics_base);

		static void Render();

	};

	class PostProcessing
	{
		
		GraphicsBase &graphics_base_;

		TextureCache &texture_cache_;

		ProgramCache &program_cache_;

		SamplerCache &sampler_cache_;

		BlendCache &blend_cache_;

		FrameBufferCache &frame_buffer_cache_;

		//std::vector<PostProcessEffect> effects_;

	public:
		
		PostProcessing(GraphicsBase &graphics_base, TextureCache &texture_cache, 
			ProgramCache &program_cache, 
			SamplerCache &sampler_cache, 
			BlendCache &blend_cache,
			FrameBufferCache &frame_buffer_cache);

		~PostProcessing();

		void Add(const PostProcessEffect &effect);

		void Process();

	};
}
