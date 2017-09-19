#include "gbuffer_comp.h"

#include "glm/glm.hpp"

#include "renderer.h"
#include "timing.h"

using namespace graphics;

GbufferComp::GbufferComp()
{
}

GbufferComp::~GbufferComp()
{
}

void GbufferComp::Init()
{
	auto &program_cache = ResourceManager::Get().GetProgramCache();
	auto &frame_buffer_cache = ResourceManager::Get().GetFrameBufferCache();
	vertex_shader_ = &program_cache.GetFromFile(v_name_, GL_VERTEX_SHADER, v_path_);
	fragment_shader_ = &program_cache.GetFromFile(f_name_, GL_FRAGMENT_SHADER, f_path_);
	pipeline_.SetStages(*vertex_shader_);
	pipeline_.SetStages(*fragment_shader_);

	gbuffer_ = &frame_buffer_cache.GetFromName(Renderer::gbuffer_name);
	gbuffer_comp_ = &frame_buffer_cache.GetFromName(Renderer::gbuffer_composition_name);

	int32_t albedo_index = 0;
	int32_t position_index = 1;
	int32_t normal_index = 2;
	int32_t depth_index = 3;
	fragment_shader_->SetUniform("u_albedo", (void *)&albedo_index);
	fragment_shader_->SetUniform("u_position", (void *)&position_index);
	fragment_shader_->SetUniform("u_normal", (void *)&normal_index);
	fragment_shader_->SetUniform("u_depth", (void*)&depth_index);
}

void GbufferComp::Apply()
{

	gbuffer_->UnbindDraw();
	gbuffer_comp_->BindDraw(GL_COLOR_BUFFER_BIT, 0, 0, 0, 1);

	int32_t albedo_index = 0;
	int32_t position_index = 1;
	int32_t normal_index = 2;
	int32_t depth_index = 3;

	gbuffer_->BindColorAttachment(albedo_index, albedo_index);
	gbuffer_->BindColorAttachment(position_index, position_index);
	gbuffer_->BindColorAttachment(normal_index, normal_index);
	gbuffer_->BindDepthStencilAttachment(depth_index);

	auto &camera = graphics_base_->GetCamera();
	vertex_shader_->SetUniform("u_view",
		(void *)glm::value_ptr(camera.GetView()));

	fragment_shader_->SetUniform("u_view",
		(void *)glm::value_ptr(camera.GetView()));

	pipeline_.Bind(); 
	this->Render();
	pipeline_.Unbind();

	gbuffer_->UnbindColorAttachment(albedo_index);
	gbuffer_->UnbindColorAttachment(position_index);
	gbuffer_->UnbindColorAttachment(normal_index);
	gbuffer_->UnbindDepthStencilAttachment();

	gbuffer_comp_->UnbindDraw();
}
