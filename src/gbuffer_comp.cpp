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
	int32_t shadow_index = 3;
	int32_t depth_index = 4;
	int32_t skybox_index = 5;

	fragment_shader_->SetUniform("u_albedo", (void *)&albedo_index);
	fragment_shader_->SetUniform("u_position", (void *)&position_index);
	fragment_shader_->SetUniform("u_normal", (void *)&normal_index);
	fragment_shader_->SetUniform("u_shadow", (void *)&shadow_index);
	fragment_shader_->SetUniform("u_depth", (void*)&depth_index);
	fragment_shader_->SetUniform("u_skybox", (void*)&skybox_index);
	
	auto &texture_cache = ResourceManager::Get().GetTextureCache();
	auto &texture = texture_cache.GetFromFile("assets/textures/violentdays_large.png", 0); 

	uint8_t * pos_x_data = texture.GetSubresourceData(2048, 1024, 1024, 1024);
	uint8_t * neg_x_data = texture.GetSubresourceData(0, 1024, 1024, 1024);
	uint8_t * pos_y_data = texture.GetSubresourceData(1024, 0, 1024, 1024);
	uint8_t * neg_y_data = texture.GetSubresourceData(1024, 2048, 1024, 1024);
	uint8_t * pos_z_data = texture.GetSubresourceData(1024, 1024, 1024, 1024);
	uint8_t * neg_z_data = texture.GetSubresourceData(3072, 1024, 1024, 1024);

	glGenTextures(1, &skybox_);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pos_x_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)neg_x_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pos_y_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)neg_y_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pos_z_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)neg_z_data);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	delete pos_x_data;
	delete neg_x_data;
	delete pos_y_data;
	delete neg_y_data;
	delete pos_z_data;
	delete neg_z_data;
}

void GbufferComp::Apply()
{

	gbuffer_->UnbindDraw();
	gbuffer_comp_->BindDraw(GL_COLOR_BUFFER_BIT, 0, 0, 0, 1);

	int32_t albedo_index = 0;
	int32_t position_index = 1;
	int32_t normal_index = 2;
	int32_t shadow_index = 3;
	int32_t depth_index = 4;
	int32_t skybox_index = 5;

	gbuffer_->BindColorAttachment(albedo_index, albedo_index);
	gbuffer_->BindColorAttachment(position_index, position_index);
	gbuffer_->BindColorAttachment(normal_index, normal_index);
	gbuffer_->BindColorAttachment(shadow_index, shadow_index);
	gbuffer_->BindDepthStencilAttachment(depth_index);

	glActiveTexture(GL_TEXTURE0 + skybox_index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_); 

	auto * camera = graphics_base_->GetMainCamera();
	vertex_shader_->SetUniform("u_view",
		(void *)glm::value_ptr(camera->GetView()));
	fragment_shader_->SetUniform("u_view",
		(void *)glm::value_ptr(camera->GetView()));
	fragment_shader_->SetUniform("u_inv_view",
		(void *)glm::value_ptr(camera->GetInvView()));
	fragment_shader_->SetUniform("u_inv_proj",
		(void *)glm::value_ptr(camera->GetInvProj()));
	 
	pipeline_.Bind(); 
	this->Render();
	pipeline_.Unbind();

	gbuffer_->UnbindColorAttachment(albedo_index);
	gbuffer_->UnbindColorAttachment(position_index);
	gbuffer_->UnbindColorAttachment(normal_index);
	gbuffer_->UnbindColorAttachment(shadow_index);
	gbuffer_->UnbindDepthStencilAttachment();

	glActiveTexture(GL_TEXTURE0 + skybox_index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	gbuffer_comp_->UnbindDraw();
}
