#pragma once

#include "frame_buffer.h"

#include "post_processing.h"

namespace graphics
{
	class PostEffect
	{
	public:

		PostEffect() = default;

		~PostEffect() = default;

		virtual void Effect(FrameBuffer * source,
			FrameBuffer * destination,
			TextureCache &texture_cache,
			ProgramCache &program_cache,
			SamplerCache &sampler_cache,
			BlendCache &blend_cache,
			FrameBufferCache &frame_buffer_cache) = 0;
	};
}
