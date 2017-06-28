#pragma once

#include <memory>

#include "graphics.h"
#include "frame_buffer.h"
//#include "shader.h"
//#include "texture.h"
//#include "sampler.h"
//#include "blend_mode.h"
//#include "sprite_renderer.h"
//#include "post_processing.h"

//#define SPRITE_INSTANCES_PER_BATCH 4000

namespace graphics
{
	class Renderer
	{

		GraphicsBase *graphics_base_;

		std::unique_ptr<FrameBuffer> offscreen_buffer_;
		//std::unique_ptr<TextureCache> texture_cache_;
		//std::unique_ptr<SamplerCache> sampler_cache_;
		//std::unique_ptr<ProgramCache> program_cache_;
		//std::unique_ptr<BlendCache> blend_cache_;
		//std::unique_ptr<PostProcessing> post_processing_;
		//std::unique_ptr<SpriteRenderer> sprite_renderer_;

	public:

		Renderer(GraphicsBase *graphics_base);

		~Renderer();

		//inline SpriteRenderer &GetSpriteRenderer() { return *sprite_renderer_; }

		//inline PostProcessing &GetPostProcessing() { return *post_processing_; }

	};
}
