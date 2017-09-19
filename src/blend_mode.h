#pragma once

#include <unordered_map>

#include "GL/glew.h"

namespace graphics
{
	enum class BlendMode
	{
		One = GL_ONE,
		SrcAlpha = GL_SRC_ALPHA,
		DstAlpha = GL_DST_ALPHA,
		SrcColor = GL_SRC_COLOR,
		DstColor = GL_DST_COLOR,
		OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
		OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
		OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
		OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
	};

	class Blend
	{

		BlendMode src_color_blend_ = BlendMode::One;
		BlendMode dst_color_blend_ = BlendMode::One;
		BlendMode src_alpha_blend_ = BlendMode::One;
		BlendMode dst_alpha_blend_ = BlendMode::One;

	public:
		
		Blend();

		Blend(BlendMode src_color_blend, BlendMode dst_color_blend,
			BlendMode src_alpha_blend, BlendMode dst_alpha_blend);

		Blend(BlendMode src_blend, BlendMode dst_blend);

		Blend(const Blend &);

		Blend(Blend &&);

		Blend &operator=(const Blend &other)
		{
			src_color_blend_ = other.src_color_blend_;
			dst_color_blend_ = other.dst_color_blend_;
			src_alpha_blend_ = other.src_alpha_blend_;
			dst_alpha_blend_ = other.dst_alpha_blend_;
		}

		Blend &operator=(Blend &&other)
		{
			src_color_blend_ = other.src_color_blend_;
			dst_color_blend_ = other.dst_color_blend_;
			src_alpha_blend_ = other.src_alpha_blend_;
			dst_alpha_blend_ = other.dst_alpha_blend_;
		}

			
		~Blend() {}

		void Set();

		inline BlendMode GetSrcColorBlendMode() const { return src_color_blend_; }

		inline BlendMode GetDstColorBlendMode() const { return dst_color_blend_; }

		inline BlendMode GetSrcAlphaBlendMode() const { return src_alpha_blend_; }

		inline BlendMode GetDstAlphaBlendMode() const { return dst_alpha_blend_; }

	};
	
	class BlendCache
	{

		std::unordered_map<size_t, Blend> blends_;

	public:

		BlendCache();

		~BlendCache();

		Blend &GetFromParameters(BlendMode src_color_blend, BlendMode src_alpha_blend,
			BlendMode dst_color_blend, BlendMode dst_alpha_blend, size_t * hash = nullptr);

		Blend &GetFromParameters(BlendMode src_blend, BlendMode dst_blend, size_t * hash = nullptr);

		Blend &GetFromHash(size_t hash);

	};
}

