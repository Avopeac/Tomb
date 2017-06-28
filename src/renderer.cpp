#include "renderer.h"

using namespace graphics;

#define SPRITE_INSTANCES_PER_BATCH 4000

Renderer::Renderer(GraphicsBase * graphics_base) :
	graphics_base_(graphics_base),
	sprite_instances_per_batch_(SPRITE_INSTANCES_PER_BATCH)
{
	texture_cache_ = std::make_unique<TextureCache>();
	sampler_cache_ = std::make_unique<SamplerCache>();
	program_cache_ = std::make_unique<ProgramCache>();
	blend_cache_ = std::make_unique<BlendCache>();

	post_processing_ = std::make_unique<PostProcessing>(*texture_cache_,
		*program_cache_, *sampler_cache_, *blend_cache_);

	sprite_renderer_ = std::make_unique<SpriteRenderer>(sprite_instances_per_batch_, 
		graphics_base_, *program_cache_, *texture_cache_, *sampler_cache_, *blend_cache_);
}

Renderer::~Renderer()
{
}

