#pragma once

#include <string>
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

	public:

		Texture();

		~Texture();

		Texture(const Texture &) = delete;

		Texture(Texture &&);

		Texture &operator=(const Texture &other) = delete;

		Texture &operator=(Texture &&other)
		{
			unit_ = other.unit_;
			id_ = other.id_;
		}

		void Bind(GLuint unit);

		void Unbind();

		void Create(const std::string &path, bool mips);

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

		Texture &CreateFromFile(size_t &hash, const std::string &path);

		Texture &GetTextureFromPath(const std::string &path);

		Texture &GetTextureFromHash(size_t hash);

	};
}

