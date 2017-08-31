#include "renderer.h"

#include "msaa_resolve.h"
#include "postfx.h"

using namespace graphics;

const std::string Renderer::offscreen_msaa_name = "offscreen_4x_msaa";
const std::string Renderer::offscreen_resolve_name = "offscreen_4x_resolve";

Renderer::Renderer(GraphicsBase *graphics_base) :
	graphics_base_(graphics_base)
{
	program_cache_ = std::make_unique<ProgramCache>();
	texture_cache_ = std::make_unique<TextureCache>();
	sampler_cache_ = std::make_unique<SamplerCache>();
	blend_cache_ = std::make_unique<BlendCache>();
	frame_buffer_cache_ = std::make_unique<FrameBufferCache>();
	mesh_cache_ = std::make_unique<MeshCache>();

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

	FrameBufferAttachmentDescriptor depth_stencil_descriptor;
	depth_stencil_descriptor.format = GL_DEPTH_COMPONENT;
	depth_stencil_descriptor.internal_format = GL_DEPTH_COMPONENT16;
	depth_stencil_descriptor.type = GL_FLOAT;

	FrameBufferAttachmentDescriptor depth_stencil_color_descriptor;
	depth_stencil_color_descriptor.format = GL_RED;
	depth_stencil_color_descriptor.internal_format = GL_R16F;
	depth_stencil_color_descriptor.type = GL_FLOAT;

	descriptors.push_back(descriptor);

	size_t h;

	msaa_fb_ = &frame_buffer_cache_->GetFromParameters(offscreen_msaa_name, h,
		graphics_base_->GetBackbufferWidth(), graphics_base_->GetBackbufferHeight(),
		4, descriptors, &depth_stencil_descriptor);
	
	descriptors.push_back(depth_stencil_color_descriptor);

	resolve_fb_ = &frame_buffer_cache_->GetFromParameters(offscreen_resolve_name, h,
		graphics_base_->GetBackbufferWidth(), graphics_base_->GetBackbufferHeight(),
		0, descriptors, nullptr);

	post_processing_->Add(std::move(std::make_unique<MsaaResolve>()));
	post_processing_->Add(std::move(std::make_unique<PostFx>()));

	//cube_ = std::make_unique<Cube>(*graphics_base_, *texture_cache_,
		//*sampler_cache_, *frame_buffer_cache_, *program_cache_, *blend_cache_);

	//cube_->Init();

	mesh_renderer_ = std::make_unique<MeshRenderer>(*graphics_base_, *program_cache_, *texture_cache_,
		*sampler_cache_, *blend_cache_, *mesh_cache_);

	mesh_renderer_->Push("cube", "assets/models/sphere.obj");
}

Renderer::~Renderer()
{
}

void Renderer::Invoke(float frame_time)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);

	// Draw sprites to MSAA offscreen buffer
	msaa_fb_->BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0, 0, 0, 1);
	mesh_renderer_->Draw(frame_time);
	//cube_->Render(frame_time);
	//sprite_renderer_->Draw();
	//font_renderer_->Draw();
	msaa_fb_->UnbindDraw();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	post_processing_->Process();
}