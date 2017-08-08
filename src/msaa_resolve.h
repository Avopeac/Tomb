#pragma once

#include "post_processing.h"

namespace graphics
{
	class MsaaResolve : public PostProcessEffect
	{

		Program vertex_shader_;
		Program fragment_shader_;
		ProgramPipeline pipeline_;

	public:

		MsaaResolve();

		~MsaaResolve();

		// Inherited via PostProcessEffect
		virtual void Init(TextureCache & texture_cache, 
			ProgramCache & program_cache, 
			SamplerCache & sampler_cache, 
			BlendCache & blend_cache, 
			FrameBufferCache & frame_buffer_cache) override;

		// Inherited via PostProcessEffect
		virtual FrameBuffer * Apply(TextureCache & texture_cache,
			ProgramCache & program_cache,
			SamplerCache & sampler_cache,
			BlendCache & blend_cache,
			FrameBufferCache & frame_buffer_cache) override;

	};
}
