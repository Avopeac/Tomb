#include "msaa_resolve.h"

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
	size_t v, f;
	vertex_shader_ = program_cache.GetFromFile("msaa_resolve.vert", v, GL_VERTEX_SHADER, 
		"assets/shaders/msaa_resolve.vert");
	fragment_shader_ = program_cache.GetFromFile("msaa_resolve.frag", f, GL_FRAGMENT_SHADER, 
		"assets/shaders/msaa_resolve.frag");

	pipeline_.SetStages(vertex_shader_);
	pipeline_.SetStages(fragment_shader_);
}

FrameBuffer * MsaaResolve::Apply(TextureCache & texture_cache, 
	ProgramCache & program_cache, 
	SamplerCache & sampler_cache, 
	BlendCache & blend_cache, 
	FrameBufferCache & frame_buffer_cache)
{

	auto &fbo0 = frame_buffer_cache.GetFromName("offscreen_4x_msaa");
	auto &fbo1 = frame_buffer_cache.GetFromName("offscreen_4x_resolve");

	fbo0.UnbindDraw();
	fbo1.UnbindRead();

	fbo0.BindRead();
	fbo1.BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0, 0, 0, 1);
	
	pipeline_.Bind();

	size_t attachments = fbo0.GetColorAttachmentCount();

	for (size_t i = 0; i < attachments; ++i)
	{
		fbo0.BindColorAttachment(i, i);
		std::string name = "color_attach" + std::to_string(i);
		glProgramUniform1i(fragment_shader_.id, glGetUniformLocation(fragment_shader_.id,
			name.c_str()), i);
	}
	
	if (fbo0.HasDepthStencil())
	{
		fbo0.BindDepthStencilAttachment(attachments);
		std::string name = "depth_attach";
		glProgramUniform1i(fragment_shader_.id, glGetUniformLocation(fragment_shader_.id,
			name.c_str()), attachments);
	}

	this->Render();

	pipeline_.Unbind();

	fbo0.UnbindRead();
	fbo1.UnbindDraw();

	return &fbo1;
}


