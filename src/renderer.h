#pragma once

#include <memory>

#include "graphics.h"
#include "shader.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"
#include "sprite_renderer.h"
#include "post_processing.h"

namespace graphics
{
	class Renderer
	{

		const size_t sprite_instances_per_batch_;

		GraphicsBase * graphics_base_;

		std::unique_ptr<TextureCache> texture_cache_;
		std::unique_ptr<SamplerCache> sampler_cache_;
		std::unique_ptr<ProgramCache> program_cache_;
		std::unique_ptr<BlendCache> blend_cache_;
		std::unique_ptr<PostProcessing> post_processing_;
		std::unique_ptr<SpriteRenderer> sprite_renderer_;

	public:

		Renderer(GraphicsBase * graphics_base);

		~Renderer();

		inline SpriteRenderer &GetSpriteRenderer() { return *sprite_renderer_; }

		inline PostProcessing &GetPostProcessing() { return *post_processing_; }

	};
}
