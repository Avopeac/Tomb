#include "renderer.h"

using namespace graphics;

Renderer::Renderer(GraphicsBase *graphics_base) :
	graphics_base_(graphics_base)
{

	//post_processing_ = std::make_unique<PostProcessing>(*texture_cache_,
		//*program_cache_, *sampler_cache_, *blend_cache_);

	//sprite_renderer_ = std::make_unique<SpriteRenderer>(SPRITE_INSTANCES_PER_BATCH, 
		//graphics_base_, *program_cache_, *texture_cache_, *sampler_cache_, *blend_cache_);

	//offscreen_buffer_ = std::make_unique<FrameBuffer>(graphics_base_->GetBackbufferWidth(),
		//graphics_base_->GetBackbufferHeight(), true, true);
}

Renderer::~Renderer()
{
}

