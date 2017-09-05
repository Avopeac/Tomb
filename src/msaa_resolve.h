#pragma once

#include "post_processing.h"

namespace graphics
{
	class MsaaResolve : public PostProcessEffect
	{

		const std::string v_name_ = "msaa_resolve.vert";
		const std::string f_name_ = "msaa_resolve.frag";
		const std::string v_path_ = "assets/shaders/msaa_resolve.vert";
		const std::string f_path_ = "assets/shaders/msaa_resolve.frag";

		Program * vertex_shader_;
		Program * fragment_shader_;

		ProgramPipeline pipeline_;

		FrameBuffer * msaa_fb_;
		FrameBuffer * resolve_fb_;

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
		virtual void Apply(TextureCache & texture_cache,
			ProgramCache & program_cache,
			SamplerCache & sampler_cache,
			BlendCache & blend_cache,
			FrameBufferCache & frame_buffer_cache) override;

	};
}
