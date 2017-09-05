#pragma once

#include "post_processing.h"

namespace graphics
{
	class PostFx : public PostProcessEffect
	{
		const std::string v_name_ = "postfx.vert";
		const std::string f_name_ = "postfx.frag";
		const std::string v_path_ = "assets/shaders/postfx.vert";
		const std::string f_path_ = "assets/shaders/postfx.frag";

		Program * vertex_shader_;
		Program * fragment_shader_;

		ProgramPipeline pipeline_;

		FrameBuffer * fbo0_;

	public:

		PostFx();

		~PostFx();

		// Inherited via PostProcessEffect
		virtual void Init(TextureCache & texture_cache, ProgramCache & program_cache, 
			SamplerCache & sampler_cache, BlendCache & blend_cache,
			FrameBufferCache & frame_buffer_cache) override;

		virtual void Apply(TextureCache & texture_cache, ProgramCache & program_cache, 
			SamplerCache & sampler_cache, BlendCache & blend_cache, 
			FrameBufferCache & frame_buffer_cache) override;

	};
}

