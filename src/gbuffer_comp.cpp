#include "gbuffer_comp.h"

#include "renderer.h"

#include "timing.h"

using namespace graphics;

GbufferComp::GbufferComp()
{
}

GbufferComp::~GbufferComp()
{
}

void GbufferComp::Init(TextureCache & texture_cache, ProgramCache & program_cache, SamplerCache & sampler_cache, BlendCache & blend_cache, FrameBufferCache & frame_buffer_cache)
{
	size_t h;
	vertex_shader_ = &program_cache.GetFromFile(v_name_, h, GL_VERTEX_SHADER, v_path_);
	fragment_shader_ = &program_cache.GetFromFile(f_name_, h, GL_FRAGMENT_SHADER, f_path_);
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

void GbufferComp::Apply(TextureCache & texture_cache, ProgramCache & program_cache, SamplerCache & sampler_cache, BlendCache & blend_cache, FrameBufferCache & frame_buffer_cache)
{

	gbuffer_->UnbindDraw();
	gbuffer_->BindRead();

	gbuffer_comp_->UnbindRead();
	gbuffer_comp_->BindDraw(GL_COLOR_BUFFER_BIT, 0, 0, 0, 1);

	int32_t albedo_index = 0;
	int32_t position_index = 1;
	int32_t normal_index = 2;
	int32_t depth_index = 3;

	gbuffer_->BindColorAttachment(albedo_index, albedo_index);
	gbuffer_->BindColorAttachment(position_index, position_index);
	gbuffer_->BindColorAttachment(normal_index, normal_index);
	gbuffer_->BindDepthStencilAttachment(depth_index);

	pipeline_.Bind();
	this->Render();
	pipeline_.Unbind();

	gbuffer_->UnbindColorAttachment(albedo_index);
	gbuffer_->UnbindColorAttachment(position_index);
	gbuffer_->UnbindColorAttachment(normal_index);
	gbuffer_->UnbindDepthStencilAttachment();

	gbuffer_->UnbindRead();
	gbuffer_comp_->UnbindDraw();
}
