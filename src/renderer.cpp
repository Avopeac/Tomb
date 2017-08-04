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

	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor descriptor;
	descriptor.format = GL_RGBA;
	descriptor.internal_format = GL_RGBA16F;
	descriptor.type = GL_FLOAT;

	descriptors.push_back(descriptor);

	FrameBufferAttachmentDescriptor depth_stencil_descriptor;
	depth_stencil_descriptor.format = GL_DEPTH_COMPONENT;
	depth_stencil_descriptor.internal_format = GL_DEPTH_COMPONENT16;
	depth_stencil_descriptor.type = GL_FLOAT;

	offscreen_buffer_msaa_ = std::make_unique<FrameBuffer>(graphics_base_->GetBackbufferWidth(),
		graphics_base_->GetBackbufferHeight(), 4, descriptors, &depth_stencil_descriptor);

	offscreen_buffer_resolved_ = std::make_unique<FrameBuffer>(graphics_base_->GetBackbufferWidth(),
		graphics_base_->GetBackbufferHeight(), 0, descriptors, &depth_stencil_descriptor);

}

Renderer::~Renderer()
{
}

void Renderer::Invoke()
{
	// Clear current framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Set depth test to false 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Enable blending
	glEnable(GL_BLEND);

	// Draw sprites to offscreen buffer
	offscreen_buffer_msaa_->BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0, 0, 0, 1);
	sprite_renderer_->Draw();
	font_renderer_->Draw();
	offscreen_buffer_msaa_->UnbindDraw();

	// TODO: Font, GUI and post processing happens between here

	// Blit offscreen buffer to default framebuffer
	/*offscreen_buffer_msaa_->Blit(0, graphics_base_->GetBackbufferWidth(), 0, graphics_base_->GetBackbufferHeight(),
		0, graphics_base_->GetBackbufferWidth(), 0, graphics_base_->GetBackbufferHeight(), offscreen_buffer_resolved_.get());*/

	offscreen_buffer_msaa_->Blit(0, graphics_base_->GetBackbufferWidth(), 0, graphics_base_->GetBackbufferHeight(),
		0, graphics_base_->GetBackbufferWidth(), 0, graphics_base_->GetBackbufferHeight(), nullptr);


	post_processing_->Process(offscreen_buffer_resolved_.get());
}

