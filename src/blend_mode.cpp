#include "blend_mode.h"

#include "SDL.h"

using namespace graphics;

Blend::Blend(BlendMode src_blend, BlendMode dst_blend)
	: src_color_blend_(src_blend), dst_color_blend_(dst_blend),
	src_alpha_blend_(src_blend), dst_alpha_blend_(dst_blend)
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

Blend &BlendCache::GetFromParameters(size_t & hash, BlendMode src_color_blend, BlendMode src_alpha_blend, 
	BlendMode dst_color_blend, BlendMode dst_alpha_blend)
{
	hash = static_cast<GLenum>(src_color_blend) | static_cast<GLenum>(dst_color_blend);
	hash ^= static_cast<GLenum>(src_alpha_blend) | static_cast<GLenum>(dst_alpha_blend);

	if (blends_.find(hash) == blends_.end())
	{
		Blend blend(src_color_blend, dst_color_blend,
			src_alpha_blend, dst_alpha_blend);

		blends_.insert({ hash, std::move(blend) });

	}

	return blends_[hash];
}

Blend & BlendCache::GetFromParameters(size_t & hash, BlendMode src_blend, BlendMode dst_blend)
{
	return GetFromParameters(hash, src_blend, src_blend, dst_blend, dst_blend);
}

Blend & BlendCache::GetFromHash(size_t hash)
{
	SDL_assert(blends_.find(hash) != blends_.end());

	return blends_[hash];
}
