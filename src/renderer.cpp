#include "renderer.h"
#include "resource_manager.h"
#include "postfx.h"

using namespace graphics;

Renderer::Renderer(GraphicsBase *graphics_base) :
	graphics_base_(graphics_base)
{

	mesh_renderer_ = std::make_unique<MeshRenderer>(*graphics_base_);

	post_processing_ = std::make_unique<PostProcessing>(*graphics_base_);
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
