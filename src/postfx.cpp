#include "postfx.h"

#include "renderer.h"

#include "timing.h"

using namespace graphics;

PostFx::PostFx()
{
}

PostFx::~PostFx()
{
}

void PostFx::Init(TextureCache & texture_cache, ProgramCache & program_cache, 
	SamplerCache & sampler_cache, BlendCache & blend_cache, 
	FrameBufferCache & frame_buffer_cache)
{
	size_t h;

	vertex_shader_ = &program_cache.GetFromFile(v_name_, h, GL_VERTEX_SHADER, v_path_);
	fragment_shader_ = &program_cache.GetFromFile(f_name_, h, GL_FRAGMENT_SHADER, f_path_);

	pipeline_.SetStages(*vertex_shader_);
	pipeline_.SetStages(*fragment_shader_);

	fbo0_ = &frame_buffer_cache.GetFromName(Renderer::offscreen_resolve_name);

	size_t num_attachments = fbo0_->GetColorAttachmentCount();

	for (int i = 0; i < (int)num_attachments; ++i)
	{
		std::string name = "u_texture" + std::to_string(i);
		glProgramUniform1i(fragment_shader_->id,
			glGetUniformLocation(fragment_shader_->id, name.c_str()), i);
	}
}

void PostFx::Apply(TextureCache & texture_cache, ProgramCache & program_cache, 
	SamplerCache & sampler_cache, BlendCache & blend_cache, 
	FrameBufferCache & frame_buffer_cache)
{
	
	fbo0_->UnbindDraw();
	fbo0_->BindRead();

	size_t num_attachments = fbo0_->GetColorAttachmentCount();

	for (int i = 0; i < (int)num_attachments; ++i)
	{
		fbo0_->BindColorAttachment(i, i);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
	 
	glProgramUniform1f(fragment_shader_->id,
		glGetUniformLocation(fragment_shader_->id, "u_time"), (float)util::GetSeconds());
	 
	pipeline_.Bind();
	this->Render();
	pipeline_.Unbind();  

	fbo0_->UnbindRead(); 
} 
