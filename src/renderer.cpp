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

	font_renderer_ = std::make_unique<FontRendererIndividual>(*graphics_base,
		*program_cache_, *texture_cache_, *sampler_cache_, *blend_cache_);

	offscreen_buffer_ = std::make_unique<FrameBuffer>(graphics_base_->GetBackbufferWidth(),
		graphics_base_->GetBackbufferHeight(), true, true);
}

Renderer::~Renderer()
{
}

void Renderer::Invoke()
{
	// Clear current framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Set depth test to false 
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_NEVER);

	// Enable blending
	glEnable(GL_BLEND);

	// Draw sprites to offscreen buffer
	offscreen_buffer_->BindDraw();
	sprite_renderer_->Draw();
	//post_processing_->Process(*offscreen_buffer_);
	font_renderer_->Draw();
	offscreen_buffer_->UnbindDraw();

	// TODO: Font, GUI and post processing happens between here

	// Blit offscreen buffer to default framebuffer
	offscreen_buffer_->Blit(0, graphics_base_->GetBackbufferWidth(), 0, graphics_base_->GetBackbufferHeight(),
		0, graphics_base_->GetBackbufferWidth(), 0, graphics_base_->GetBackbufferHeight());

}

