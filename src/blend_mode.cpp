#include "blend_mode.h"

#include "SDL.h"

#include "logger.h"

using namespace graphics;

Blend::Blend(BlendMode src_blend, BlendMode dst_blend)
	: src_color_blend_(src_blend), dst_color_blend_(dst_blend),
	src_alpha_blend_(src_blend), dst_alpha_blend_(dst_blend)
{
}

graphics::Blend::Blend()
	: src_color_blend_(BlendMode::One), dst_color_blend_(BlendMode::One),
	src_alpha_blend_(BlendMode::One), dst_alpha_blend_(BlendMode::One)
{
}

Blend::Blend(BlendMode src_color_blend, BlendMode dst_color_blend,
	BlendMode src_alpha_blend, BlendMode dst_alpha_blend)
	: src_color_blend_(src_color_blend), dst_color_blend_(dst_color_blend),
	src_alpha_blend_(src_alpha_blend), dst_alpha_blend_(dst_alpha_blend)
{
}

Blend::Blend(const Blend &other) 
	: src_color_blend_(other.src_color_blend_), dst_color_blend_(other.dst_color_blend_),
	src_alpha_blend_(other.src_alpha_blend_), dst_alpha_blend_(other.dst_alpha_blend_)
{
}

Blend::Blend(Blend &&other) 
	: src_color_blend_(other.src_color_blend_), dst_color_blend_(other.dst_color_blend_),
	src_alpha_blend_(other.src_alpha_blend_), dst_alpha_blend_(other.dst_alpha_blend_)
{
}

void Blend::Set()
{
	glBlendFuncSeparate(static_cast<GLenum>(src_color_blend_),
		static_cast<GLenum>(dst_color_blend_),
		static_cast<GLenum>(src_alpha_blend_),
		static_cast<GLenum>(dst_alpha_blend_));
}

BlendCache::BlendCache()
{
}

BlendCache::~BlendCache()
{
}

Blend * BlendCache::GetFromParameters(BlendMode src_color_blend, BlendMode src_alpha_blend, 
	BlendMode dst_color_blend, BlendMode dst_alpha_blend, size_t * hash)
{
	size_t name_hash = static_cast<GLenum>(src_color_blend) | static_cast<GLenum>(dst_color_blend);
	name_hash ^= static_cast<GLenum>(src_alpha_blend) | static_cast<GLenum>(dst_alpha_blend);

	if (blends_.find(name_hash) == blends_.end())
	{
		Blend blend(src_color_blend, dst_color_blend,
			src_alpha_blend, dst_alpha_blend);

		blends_.insert({ name_hash , std::move(blend) });

	}

	if (hash)
	{
		*hash = name_hash;
	}

	return &blends_[name_hash];
}

Blend * BlendCache::GetFromParameters(BlendMode src_blend, BlendMode dst_blend, size_t * hash)
{
	return GetFromParameters(src_blend, src_blend, dst_blend, dst_blend, hash);
}

Blend * BlendCache::GetFromHash(size_t hash)
{
	if (blends_.find(hash) != blends_.end())
	{
		return &blends_[hash];
	}

	debug::Log(SDL_LOG_PRIORITY_CRITICAL, SDL_LOG_CATEGORY_RENDER, "Blend state is null.");
	return nullptr;
}
