#pragma once

#include <unordered_map>

#include "GL/glew.h"

#include "SDL.h"

#include "disposable.h"

namespace graphics
{
	enum class MinificationFiltering : Uint32
	{
		Linear = GL_LINEAR,
		LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
		LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
		Nearest = GL_NEAREST,
		NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
		NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
	};

	enum class MagnificationFiltering : Uint32
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR,
	};

	enum class Wrapping : Uint32
	{
		Repeat = GL_REPEAT,
		ClampToEdge = GL_CLAMP_TO_EDGE,
		MirroredRepeat = GL_MIRRORED_REPEAT,
		MirroredClampToEdge = GL_MIRROR_CLAMP_TO_EDGE,
	};

	class Sampler : public base::Disposable
	{
		Uint32 unit_;

		GLuint id_;

	public:

		Sampler();

		~Sampler();

		Sampler(const Sampler &) = delete;

		Sampler(const Sampler &&);

		Sampler &operator=(const Sampler &) = delete;

		Sampler &operator=(Sampler &&other)
		{
			if (&other != this)
			{
				unit_ = other.unit_;
				id_ = other.id_;
				other.unit_ = 0;
				other.id_ = 0;
			}
		}

		void SetFiltering(MagnificationFiltering mag,
			MinificationFiltering min);

		void SetWrap(Wrapping s, Wrapping t);

		void Bind(Uint32 unit);

		void Unbind();

		// Inherited via Disposable
		virtual void Free() override;
	};

	class SamplerCache
	{
		std::unordered_map<size_t, Sampler> samplers_;

	public:

		SamplerCache();

		~SamplerCache();

		Sampler &GetFromParameters(size_t &hash, MagnificationFiltering magnification,
			MinificationFiltering minification, Wrapping s, Wrapping t);

		Sampler &GetFromHash(size_t hash);
	};
}