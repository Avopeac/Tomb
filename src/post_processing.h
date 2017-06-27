#pragma once

#include <functional>
#include <vector>
#include <algorithm>

#include "shader.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"

namespace graphics
{

	class FrameBuffer
	{
	public:

		FrameBuffer(){}

		~FrameBuffer() {}
	};

	class PostProcessing
	{

	private:

		TextureCache &texture_cache_;

		ProgramCache &program_cache_;

		SamplerCache &sampler_cache_;

		BlendCache &blend_cache_;

		std::vector<std::function<FrameBuffer &(FrameBuffer source,
			TextureCache &texture_cache,
			ProgramCache &program_cache,
			SamplerCache &sampler_cache,
			BlendCache &blend_cache)>> effects_;

	public:
		
		PostProcessing(TextureCache &texture_cache, 
			ProgramCache &program_cache, 
			SamplerCache &sampler_cache, 
			BlendCache &blend_cache);

		~PostProcessing();

		void Add(std::function<FrameBuffer &(FrameBuffer source,
			TextureCache &texture_cache,
			ProgramCache &program_cache,
			SamplerCache &sampler_cache,
			BlendCache &blend_cache)> effect);

		FrameBuffer &Process(FrameBuffer framebuffer);

	};
}
