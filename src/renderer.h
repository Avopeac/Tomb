#pragma once

#include <memory>

#include "graphics.h"
#include "frame_buffer.h"
#include "shader.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"
#include "sprite_renderer.h"
//#include "font_renderer.h"
#include "font_renderer_individual.h"
#include "post_processing.h"

#define SPRITE_INSTANCES_PER_BATCH 4000

namespace graphics
{
	class Renderer
	{

		GraphicsBase *graphics_base_;

		std::unique_ptr<FrameBufferCache> frame_buffer_cache_;
		std::unique_ptr<TextureCache> texture_cache_;
		std::unique_ptr<SamplerCache> sampler_cache_;
		std::unique_ptr<ProgramCache> program_cache_;
		std::unique_ptr<BlendCache> blend_cache_;
		std::unique_ptr<PostProcessing> post_processing_;
		std::unique_ptr<SpriteRenderer> sprite_renderer_;
		//std::unique_ptr<FontRenderer> font_renderer_;
		std::unique_ptr<FontRendererIndividual> font_renderer_;

		size_t offscreen_4x_msaa_hash_;
		size_t offscreen_4x_resolve_hash_;

	public:

		Renderer(GraphicsBase *graphics_base);

		~Renderer();

		inline SpriteRenderer &GetSpriteRenderer() { return *sprite_renderer_; }

		inline FontRendererIndividual &GetFontRenderer() { return *font_renderer_; }

		inline PostProcessing &GetPostProcessing() { return *post_processing_; }

		void Invoke();

	};
}
