#include "renderer.h"
#include "resource_manager.h"
#include "gbuffer_comp.h"
#include "postfx.h"

using namespace graphics;

const std::string Renderer::shadow_map_name = "shadow_map";
const std::string Renderer::gbuffer_name = "gbuffer";
const std::string Renderer::gbuffer_composition_name = "gbuffer_comp";

Renderer::Renderer(GraphicsBase *graphics_base) :
	graphics_base_(graphics_base)
{

	post_processing_ = std::make_unique<PostProcessing>(*graphics_base_);
	mesh_renderer_ = std::make_unique<MeshRenderer>(*graphics_base_);

	shadow_map_ = MakeShadowMap();
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

	auto * main_camera = graphics_base_->GetMainCamera();
	auto * shadow_camera = graphics_base_->GetShadowCamera();

	main_camera->Update(frame_time);
	shadow_camera->Update(frame_time);
	mesh_renderer_->Draw(frame_time);

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


	auto &frame_buffer_cache = ResourceManager::Get().GetFrameBufferCache();
	return &frame_buffer_cache.GetFromParameters(gbuffer_name,
		graphics_base_->GetBackbufferWidth(), graphics_base_->GetBackbufferHeight(), 
		0, descriptors, &depth);

}

FrameBuffer * graphics::Renderer::MakeGbufferComposition()
{
	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor composition;
	composition.format = GL_RGB;
	composition.internal_format = GL_RGB16F;
	composition.type = GL_FLOAT;

	descriptors.push_back(composition);

	auto &frame_buffer_cache = ResourceManager::Get().GetFrameBufferCache();
	return &frame_buffer_cache.GetFromParameters(gbuffer_composition_name, 
		graphics_base_->GetBackbufferWidth(), graphics_base_->GetBackbufferHeight(), 
		0, descriptors, nullptr);
}

FrameBuffer * graphics::Renderer::MakeShadowMap()
{
	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor depth;
	depth.format = GL_DEPTH_COMPONENT;
	depth.internal_format = GL_DEPTH_COMPONENT24;
	depth.type = GL_FLOAT;

	auto &frame_buffer_cache = ResourceManager::Get().GetFrameBufferCache();
	return &frame_buffer_cache.GetFromParameters(shadow_map_name,
		graphics_base_->GetBackbufferWidth(), graphics_base_->GetBackbufferHeight(),
		0, descriptors, &depth);
}
