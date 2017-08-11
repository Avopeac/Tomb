#include "passthrough.h"

using namespace graphics;

Passthrough::Passthrough()
{
}

Passthrough::~Passthrough()
{
}

void Passthrough::Init(TextureCache & texture_cache, ProgramCache & program_cache, 
	SamplerCache & sampler_cache, BlendCache & blend_cache, 
	FrameBufferCache & frame_buffer_cache)
{
	size_t v, f;

	vertex_shader_ = &program_cache.GetFromFile("passthrough.vert", v, GL_VERTEX_SHADER, "assets/shaders/passthrough.vert");
	fragment_shader_ = &program_cache.GetFromFile("passthrough.frag", f, GL_FRAGMENT_SHADER, "assets/shaders/passthrough.frag");

	pipeline_.SetStages(*vertex_shader_);
	pipeline_.SetStages(*fragment_shader_);
}

FrameBuffer * Passthrough::Apply(TextureCache & texture_cache, ProgramCache & program_cache, 
	SamplerCache & sampler_cache, BlendCache & blend_cache, 
	FrameBufferCache & frame_buffer_cache)
{
	auto &fbo = frame_buffer_cache.GetFromName("offscreen_4x_resolve");
	
	pipeline_.Bind();


	pipeline_.Unbind();

	return nullptr;
}
