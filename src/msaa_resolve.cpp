#include "msaa_resolve.h"

#include "renderer.h"

using namespace graphics;

MsaaResolve::MsaaResolve()
{
}

MsaaResolve::~MsaaResolve()
{
}

void MsaaResolve::Init(TextureCache & texture_cache,
	ProgramCache & program_cache,
	SamplerCache & sampler_cache,
	BlendCache & blend_cache,
	FrameBufferCache & frame_buffer_cache)
{
	size_t h;
	vertex_shader_ = &program_cache.GetFromFile(v_name_, h, GL_VERTEX_SHADER, v_path_);
	fragment_shader_ = &program_cache.GetFromFile(f_name_, h, GL_FRAGMENT_SHADER, f_path_);

	pipeline_.SetStages(*vertex_shader_);
	pipeline_.SetStages(*fragment_shader_);

	msaa_fb_ = &frame_buffer_cache.GetFromName(Renderer::offscreen_msaa_name);
	resolve_fb_ = &frame_buffer_cache.GetFromName(Renderer::offscreen_resolve_name);

	size_t num_attachments = msaa_fb_->GetColorAttachmentCount();

	for (int i = 0; i < (int)num_attachments; ++i)
	{
		std::string name = "u_texture" + std::to_string(i);
		glProgramUniform1i(fragment_shader_->GetId(),
			glGetUniformLocation(fragment_shader_->GetId(), name.c_str()), i);
	}

	if (msaa_fb_->HasDepthStencil())
	{
		std::string name = "u_depth_texture";
		glProgramUniform1i(fragment_shader_->GetId(),
			glGetUniformLocation(fragment_shader_->GetId(), name.c_str()), (int)num_attachments);
	}

	glProgramUniform1i(fragment_shader_->GetId(), glGetUniformLocation(fragment_shader_->GetId(),
		"u_num_samples"), msaa_fb_->GetNumSamples());

	glProgramUniform2i(fragment_shader_->GetId(), glGetUniformLocation(fragment_shader_->GetId(),
		"u_resolution"), msaa_fb_->GetWidth(), msaa_fb_->GetHeight());
}

void MsaaResolve::Apply(TextureCache & texture_cache, 
	ProgramCache & program_cache, 
	SamplerCache & sampler_cache, 
	BlendCache & blend_cache, 
	FrameBufferCache & frame_buffer_cache)
{

	msaa_fb_->UnbindDraw();
	msaa_fb_->BindRead();

	resolve_fb_->UnbindRead();
	resolve_fb_->BindDraw(GL_COLOR_BUFFER_BIT, 0, 0, 0, 1);

	size_t attachments = msaa_fb_->GetColorAttachmentCount();

	for (int i = 0; i < (int)attachments; ++i)
	{
		msaa_fb_->BindColorAttachment(i, i);
	}

	if (msaa_fb_->HasDepthStencil())
	{
		msaa_fb_->BindDepthStencilAttachment((int)attachments);
	}

	pipeline_.Bind();
	this->Render();
	pipeline_.Unbind();

	msaa_fb_->UnbindRead();
	resolve_fb_->UnbindDraw();
}


