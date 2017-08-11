#pragma once

#include "post_processing.h"

namespace graphics
{
	class Passthrough : public PostProcessEffect
	{
		const Program * vertex_shader_;
		const Program * fragment_shader_;
		ProgramPipeline pipeline_;

	public:

		Passthrough();

		~Passthrough();

		// Inherited via PostProcessEffect
		virtual void Init(TextureCache & texture_cache, ProgramCache & program_cache, 
			SamplerCache & sampler_cache, BlendCache & blend_cache,
			FrameBufferCache & frame_buffer_cache) override;

		virtual FrameBuffer * Apply(TextureCache & texture_cache, ProgramCache & program_cache, 
			SamplerCache & sampler_cache, BlendCache & blend_cache, 
			FrameBufferCache & frame_buffer_cache) override;

	};
}

