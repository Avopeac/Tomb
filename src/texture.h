#pragma once

#include <string>
#include <unordered_map>

#include "SDL.h"

#include "GL/glew.h"

namespace graphics
{

	struct Texture
	{
		GLuint id;
	};

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

	class Sampler
	{
		Uint32 unit_;
		GLuint id_;

	public:

		Sampler();

		~Sampler();

		void SetFiltering(MagnificationFiltering mag,
			MinificationFiltering min);

		void SetWrap(Wrapping s, Wrapping t);

		void Bind(Uint32 unit);

		void Unbind();
	};

	class TextureCache
	{

		std::unordered_map<size_t, Texture> textures_;

	public:

		TextureCache();

		~TextureCache();

		void CreateFromFile(const std::string &path);

		Texture &GetTextureFromPath(const std::string &path);

		Texture &GetTextureFromHash(size_t hash);

	};
}

