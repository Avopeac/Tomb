#include "renderer.h"

using namespace graphics;

Renderer::Renderer(GraphicsBase *graphics_base) :
	graphics_base_(graphics_base)
{

	program_cache_ = std::make_unique<ProgramCache>();
	texture_cache_ = std::make_unique<TextureCache>();
	sampler_cache_ = std::make_unique<SamplerCache>();
	blend_cache_ = std::make_unique<BlendCache>();

	post_processing_ = std::make_unique<PostProcessing>(*texture_cache_,
		*program_cache_, *sampler_cache_, *blend_cache_);

	sprite_renderer_ = std::make_unique<SpriteRenderer>(SPRITE_INSTANCES_PER_BATCH,
		*graphics_base_, *program_cache_, *texture_cache_, *sampler_cache_, *blend_cache_);

	offscreen_buffer_ = std::make_unique<FrameBuffer>(graphics_base_->GetBackbufferWidth(),
		graphics_base_->GetBackbufferHeight(), true, true);
}

Renderer::~Renderer()
{
}

void Renderer::Invoke()
{

	offscreen_buffer_->BindDraw();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	sprite_renderer_->Draw();

	offscreen_buffer_->UnbindDraw();

}
