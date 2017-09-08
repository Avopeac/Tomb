#include "renderer.h"

#include "gbuffer_comp.h"
#include "postfx.h"

using namespace graphics;

const std::string Renderer::gbuffer_name = "gbuffer_comp";
const std::string Renderer::gbuffer_composition_name = "gbuffer_comp";

Renderer::Renderer(GraphicsBase *graphics_base) :
	graphics_base_(graphics_base)
{
	program_cache_ = std::make_unique<ProgramCache>();
	texture_cache_ = std::make_unique<TextureCache>();
	sampler_cache_ = std::make_unique<SamplerCache>();
	blend_cache_ = std::make_unique<BlendCache>();
	mesh_cache_ = std::make_unique<MeshCache>();
	frame_buffer_cache_ = std::make_unique<FrameBufferCache>();

	post_processing_ = std::make_unique<PostProcessing>(*graphics_base_, *texture_cache_,
		*program_cache_, *sampler_cache_, *blend_cache_, *frame_buffer_cache_);
	sprite_renderer_ = std::make_unique<SpriteRenderer>(SPRITE_INSTANCES_PER_BATCH,
		*graphics_base_, *program_cache_, *texture_cache_, *sampler_cache_, *blend_cache_);
	font_renderer_ = std::make_unique<FontRendererIndividual>(*graphics_base_,
		*program_cache_, *texture_cache_, *sampler_cache_, *blend_cache_);
	mesh_renderer_ = std::make_unique<MeshRenderer>(*graphics_base_, *program_cache_, *texture_cache_,
		*sampler_cache_, *blend_cache_, *mesh_cache_);

	gbuffer_ = MakeGbuffer();
	gbuffer_comp_ = MakeGbufferComposition();

	post_processing_->Add(std::move(std::make_unique<GbufferComp>()));
	post_processing_->Add(std::move(std::make_unique<PostFx>())); 

	glEnable(GL_CULL_FACE);
	glDisable(GL_MULTISAMPLE);
	glCullFace(GL_BACK); 
}

Renderer::~Renderer()
{
}

void Renderer::Invoke(float frame_time)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	
	gbuffer_->BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0.0f, 0.0f, 0.0f, 1.0f);
	mesh_renderer_->Draw();
	gbuffer_->UnbindDraw();

	glDisable(GL_DEPTH_TEST);

	post_processing_->Process();
}

FrameBuffer * Renderer::MakeGbuffer()
{
	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor albedo;
	albedo.format = GL_RGB;
	albedo.internal_format = GL_RGB16F;
	albedo.type = GL_FLOAT;

	FrameBufferAttachmentDescriptor position;
	position.format = GL_RGB;
	position.internal_format = GL_RGB16F;
	position.type = GL_FLOAT;

	FrameBufferAttachmentDescriptor normals;
	normals.format = GL_RGB;
	normals.internal_format = GL_RGB16F;
	normals.type = GL_FLOAT;

	FrameBufferAttachmentDescriptor depth;
	depth.format = GL_DEPTH_COMPONENT;
	depth.internal_format = GL_DEPTH_COMPONENT24;
	depth.type = GL_FLOAT;

	descriptors.push_back(albedo);
	descriptors.push_back(position);
	descriptors.push_back(normals);

	size_t hash;
	return &frame_buffer_cache_->GetFromParameters(gbuffer_name, hash,
		graphics_base_->GetBackbufferWidth(), graphics_base_->GetBackbufferHeight(), 0, descriptors, &depth);

}

FrameBuffer * graphics::Renderer::MakeGbufferComposition()
{
	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor composition;
	composition.format = GL_RGB;
	composition.internal_format = GL_RGB16F;
	composition.type = GL_FLOAT;

	descriptors.push_back(composition);

	size_t hash;
	return &frame_buffer_cache_->GetFromParameters(gbuffer_composition_name, hash,
		graphics_base_->GetBackbufferWidth(), graphics_base_->GetBackbufferHeight(), 0, descriptors, nullptr);
}
