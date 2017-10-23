#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "SDL.h"
#include "GL/glew.h"

#include "disposable.h"

namespace graphics
{

	class Texture : base::Disposable
	{
		GLuint unit_;

		GLuint id_;
		
		GLenum format_;

		size_t width_, height_;

		uint8_t * data_;

	public:

		Texture();

		~Texture();

		Texture(const Texture &) = delete;

		Texture &operator=(const Texture &other) = delete;

		Texture(Texture &&other);

		Texture &operator=(Texture &&other)
		{
			if (&other != this)
			{
				id_ = other.id_;
				format_ = other.format_;
				unit_ = other.unit_;
				width_ = other.width_;
				height_ = other.height_;
				data_ = other.data_;

				other.id_ = 0;
				other.format_ = 0;
				other.unit_ = 0;
				other.width_ = 0;
				other.height_ = 0;
				other.data_ = 0;
			}
			
		}

		void Bind(GLuint unit);

		void Unbind();

		void Create(const std::string &path, bool mips);

		void Create(SDL_Surface * surface, bool mips);

		// Inherited via Disposable
		virtual void Free() override;

		inline GLuint GetId() const { return id_; }

		inline GLuint GetUnit() const { return unit_; }

		inline GLenum GetFormat() const { return format_; }

		inline size_t GetWidth() const { return width_; }

		inline size_t GetHeight() const { return height_; }

		inline const uint8_t * const GetData() const { return data_; }

		uint8_t * GetSubresourceData(size_t x, size_t y, size_t w, size_t h);

	};

	class TextureCache
	{

		std::unordered_map<size_t, Texture> textures_;

	public:

		TextureCache();

		~TextureCache();

		Texture * GetFromFile(const std::string &path, bool mips, size_t * hash = nullptr);

		Texture * GetFromHash(size_t hash);

		Texture * GetFromSurface(SDL_Surface * surface, const std::string &name, bool mips, size_t * hash = nullptr);

	};
}

