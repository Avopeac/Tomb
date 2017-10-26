#include "post_processing.h"

#include "renderer.h"

#include "fullscreen_quad.h"

using namespace graphics;

const GraphicsBase * PostProcessEffect::graphics_base_ = nullptr;

PostProcessing::PostProcessing(GraphicsBase &graphics_base)
	: graphics_base_(graphics_base)
{
	PostProcessEffect::Init(graphics_base_);
} 

PostProcessing::~PostProcessing()
{
}

void PostProcessing::Add(std::unique_ptr<PostProcessEffect> effect)
{
	effects_.push_back(std::move(effect));
	effects_.back()->Init();
}

void PostProcessing::Process()
{
	for (auto &effect : effects_)
	{
		effect->Apply();
	}
}

void PostProcessEffect::Init(const GraphicsBase &graphics_base)
{
	graphics_base_ = &graphics_base;
}

void PostProcessEffect::Render()
{	
	FullscreenQuad::Get().Begin();
	FullscreenQuad::Get().DrawElements();
	FullscreenQuad::Get().End();
}
