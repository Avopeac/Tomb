#pragma once

#pragma once

#include "post_processing.h"

namespace graphics
{
	class GbufferComp : public PostProcessEffect
	{
		const std::string v_name_ = "gbuffer_comp.vert";
		const std::string f_name_ = "gbuffer_comp.frag";
		const std::string v_path_ = "assets/shaders/gbuffer_comp.vert";
		const std::string f_path_ = "assets/shaders/gbuffer_comp.frag";

		Program * vertex_shader_;
		Program * fragment_shader_;
		ProgramPipeline pipeline_;

		FrameBuffer * gbuffer_;
		FrameBuffer * gbuffer_comp_;

	public:

		GbufferComp();

		~GbufferComp();

		// Inherited via PostProcessEffect
		virtual void Init(TextureCache & texture_cache, ProgramCache & program_cache,
			SamplerCache & sampler_cache, BlendCache & blend_cache,
			FrameBufferCache & frame_buffer_cache) override;

		virtual void Apply(TextureCache & texture_cache, ProgramCache & program_cache,
			SamplerCache & sampler_cache, BlendCache & blend_cache,
			FrameBufferCache & frame_buffer_cache) override;

	};
}


