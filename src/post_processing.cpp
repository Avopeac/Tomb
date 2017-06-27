#include "post_processing.h"

using namespace graphics;

PostProcessing::PostProcessing(TextureCache & texture_cache, 
	ProgramCache & program_cache, 
	SamplerCache & sampler_cache, 
	BlendCache & blend_cache) 
	: texture_cache_(texture_cache),
	program_cache_(program_cache),
	sampler_cache_(sampler_cache),
	blend_cache_(blend_cache)
{
}

PostProcessing::~PostProcessing()
{
}

void PostProcessing::Add(std::function<FrameBuffer &(FrameBuffer source,
	TextureCache &texture_cache,
	ProgramCache &program_cache,
	SamplerCache &sampler_cache,
	BlendCache &blend_cache)> effect)
{
	effects_.push_back(effect);
}

FrameBuffer & PostProcessing::Process(FrameBuffer framebuffer)
{
	// TODO: Implement properly
	return FrameBuffer(2, 2, false, false);
}
