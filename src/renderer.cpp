#include "renderer.h"

using namespace graphics;

Renderer::Renderer(GraphicsBase *graphics_base) :
	graphics_base_(graphics_base)
{
	program_cache_ = std::make_unique<ProgramCache>();
	texture_cache_ = std::make_unique<TextureCache>();
	sampler_cache_ = std::make_unique<SamplerCache>();
	blend_cache_ = std::make_unique<BlendCache>();
	frame_buffer_cache_ = std::make_unique<FrameBufferCache>();

	post_processing_ = std::make_unique<PostProcessing>(*graphics_base_, *texture_cache_,
		*program_cache_, *sampler_cache_, *blend_cache_, *frame_buffer_cache_);

	sprite_renderer_ = std::make_unique<SpriteRenderer>(SPRITE_INSTANCES_PER_BATCH,
		*graphics_base_, *program_cache_, *texture_cache_, *sampler_cache_, *blend_cache_);

	font_renderer_ = std::make_unique<FontRendererIndividual>(*graphics_base_,
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

	frame_buffer_cache_->GetFromParameters("offscreen_4x_msaa", offscreen_4x_msaa_hash_,
		graphics_base_->GetBackbufferWidth(),
		graphics_base_->GetBackbufferHeight(),
		4, descriptors, &depth_stencil_descriptor);

	FrameBufferAttachmentDescriptor depth_stencil_color_descriptor;
	depth_stencil_color_descriptor.format = GL_RED;
	depth_stencil_color_descriptor.internal_format = GL_R16F;
	depth_stencil_color_descriptor.type = GL_FLOAT;

	descriptors.push_back(depth_stencil_color_descriptor);

	frame_buffer_cache_->GetFromParameters("offscreen_4x_resolve", offscreen_4x_resolve_hash_,
		graphics_base_->GetBackbufferWidth(),
		graphics_base_->GetBackbufferHeight(),
		0, descriptors, nullptr);

	post_processing_->Add(std::move(std::make_unique<MsaaResolve>()));
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
	glEnable(GL_BLEND);

	// Draw sprites to MSAA offscreen buffer
	frame_buffer_cache_->GetFromHash(offscreen_4x_msaa_hash_).BindDraw(
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0, 0, 0, 1);
	sprite_renderer_->Draw();
	font_renderer_->Draw();
	frame_buffer_cache_->GetFromHash(offscreen_4x_msaa_hash_).UnbindDraw();

	// TODO: Font, GUI and post processing happens between here

	/*frame_buffer_cache_->GetFromHash(offscreen_4x_msaa_hash_).Blit(
		0, graphics_base_->GetBackbufferWidth(),
		0, graphics_base_->GetBackbufferHeight(),
		0, graphics_base_->GetBackbufferWidth(),
		0, graphics_base_->GetBackbufferHeight(),
		nullptr);*/

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	post_processing_->Process();
}