#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>

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

		PostProcessEffect() {};

		virtual ~PostProcessEffect() {};

		virtual void Init(TextureCache &texture_cache,
			ProgramCache &program_cache,
			SamplerCache &sampler_cache,
			BlendCache &blend_cache,
			FrameBufferCache &frame_buffer_cache) = 0;
 
		virtual void Apply(TextureCache &texture_cache,
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

		std::vector<std::unique_ptr<PostProcessEffect>> effects_;

	public:
		
		PostProcessing(GraphicsBase &graphics_base, TextureCache &texture_cache, 
			ProgramCache &program_cache, 
			SamplerCache &sampler_cache, 
			BlendCache &blend_cache,
			FrameBufferCache &frame_buffer_cache);

		~PostProcessing();

		void Add(std::unique_ptr<PostProcessEffect> effect);

		void Process();

	};
}
