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

void PostProcessing::Add(std::function<FrameBuffer &(FrameBuffer &source,
	TextureCache &texture_cache,
	ProgramCache &program_cache,
	SamplerCache &sampler_cache,
	BlendCache &blend_cache)> effect)
{
	effects_.push_back(effect);
}

FrameBuffer & PostProcessing::Process(FrameBuffer &framebuffer)
{
	FrameBuffer *dest = nullptr;
	FrameBuffer *src = &framebuffer;
	for (auto effect : effects_)
	{
		dest = &effect(*src, texture_cache_, program_cache_, sampler_cache_, blend_cache_);
		src = dest;
	}

	return *dest;
}
