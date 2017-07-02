#pragma once

#include <string>
#include <unordered_map>

#include "SDL.h"

#include "GL/glew.h"

#include "disposable.h"
#include "frame_buffer.h"

namespace graphics
{

	class Texture : base::Disposable
	{
		GLuint unit_;

		GLuint id_;

	public:

		Texture();

		~Texture();

		Texture(const Texture &) = delete;

		Texture &operator=(const Texture &other) = delete;

		Texture(Texture &&);

		Texture &operator=(Texture &&other)
		{
			unit_ = other.unit_;
			id_ = other.id_;
		}

		void Bind(GLuint unit);

		void Unbind();

		void Create(const std::string &path, bool mips);

		// Frees the surface afterwards
		void Create(SDL_Surface * surface, bool mips);

		// Inherited via Disposable
		virtual void Free() override;

		inline GLuint GetId() const { return id_; }

		inline GLuint GetUnit() const { return unit_; }

	};

	class TextureCache
	{

		std::unordered_map<size_t, Texture> textures_;

	public:

		TextureCache();

		~TextureCache();

		Texture &GetFromFile(size_t &hash, const std::string &path);

		Texture &GetFromHash(size_t hash);

		Texture &GetFromSurface(size_t &hash, SDL_Surface * surface, const std::string &name);

	};
}

