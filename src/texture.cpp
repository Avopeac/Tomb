#include "texture.h"

#include <functional>

#include "SDL_image.h"

#include "logger.h"

using namespace graphics;

Texture::Texture()
{
	glGenTextures(1, &id_);
}

Texture::~Texture()
{

}

Texture::Texture(Texture &&other)
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

void Texture::Bind(GLuint unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, id_);
	unit_ = unit;
}

void Texture::Unbind()
{
	glActiveTexture(GL_TEXTURE0 + unit_);
	glBindTexture(GL_TEXTURE_2D, 0);
	unit_ = 0;
}

void Texture::Create(const std::string & path, bool mips)
{
	SDL_RWops * rw = SDL_RWFromFile(path.c_str(), "rb");

	SDL_assert(rw);

	SDL_Surface * surface_ = IMG_LoadPNG_RW(rw);

	Create(surface_, mips);

	SDL_FreeRW(rw);
}

void Texture::Create(SDL_Surface * surface, bool mips)
{
	SDL_PixelFormat pixel_format;
	SDL_memset(&pixel_format, 0, sizeof(pixel_format));
	pixel_format.BitsPerPixel = surface->format->BitsPerPixel;
	pixel_format.BytesPerPixel = surface->format->BytesPerPixel;
	pixel_format.palette = surface->format->palette;
	pixel_format.format = SDL_PIXELFORMAT_RGBA8888;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	pixel_format.Rmask = 0xff000000;
	pixel_format.Gmask = 0x00ff0000;
	pixel_format.Bmask = 0x0000ff00;
	pixel_format.Amask = 0x000000ff;
	#else
	pixel_format.Rmask = 0x000000ff;
	pixel_format.Gmask = 0x0000ff00;
	pixel_format.Bmask = 0x00ff0000;
	pixel_format.Amask = 0xff000000;
	#endif

	GLint internal_format;
	if (pixel_format.BytesPerPixel == 3)
	{
		internal_format = GL_RGB;
	}
	else if (pixel_format.BytesPerPixel == 4)
	{
		internal_format = GL_RGBA;
	}

	SDL_Surface * converted_surface = SDL_ConvertSurface(surface, &pixel_format, 0);

	glBindTexture(GL_TEXTURE_2D, id_);
	glTexImage2D(GL_TEXTURE_2D, 1, internal_format, converted_surface->w, converted_surface->h,
		0, GL_RGBA, GL_UNSIGNED_BYTE, converted_surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (mips)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	width_ = (size_t)surface->w;
	height_ = (size_t)surface->h;
	format_ = internal_format;

	// Copy pixel data
	size_t data_size = surface->w * surface->h * surface->format->BytesPerPixel;
	data_ = new uint8_t[data_size];

	if (surface->format->BytesPerPixel == 4)
	{
		SDL_memcpy4(data_, surface->pixels, surface->w * surface->h);
	}
	else
	{
		SDL_memcpy(data_, surface->pixels, data_size);
	}

	SDL_FreeSurface(surface);
}

void Texture::Free()
{
	if (data_)
	{
		delete data_;
	}

	glDeleteTextures(1, &id_);
}

uint8_t * Texture::GetSubresourceData(size_t x, size_t y, size_t w, size_t h)
{
	SDL_assert((x + w) <= width_ && (y + h) <= height_);

	uint8_t * data = nullptr; 

	if (format_ == GL_RGBA)
	{
		size_t pixel_size = 4 * sizeof(uint8_t);
		data = new uint8_t[pixel_size * w * h];

		size_t offset_src = 0;
		size_t offset_dst = 0;
		for (size_t row = y; row < y + h; ++row)
		{
			SDL_memcpy4(
				(void*)(data + offset_dst), 
				(void*)(data_ + offset_src), w);

			offset_dst += w * pixel_size;
			offset_src = (x + row * width_) * pixel_size;
		}
	}
	else if (format_ == GL_RGB)
	{
		size_t pixel_size = 3 * sizeof(uint8_t);
		data = new uint8_t[pixel_size * w * h];
		size_t offset_src = 0;
		size_t offset_dst = 0;
		for (size_t row = y; row < y + h; ++row)
		{
			SDL_memcpy(
				(void*)(data + offset_dst),
				(void*)(data_ + offset_src), w);

			offset_dst += w * pixel_size;
			offset_src = (x + row * width_) * pixel_size;
		}
	}
	
	return data;
}

TextureCache::TextureCache()
{
}

TextureCache::~TextureCache()
{
	for (auto &texture : textures_)
	{
		texture.second.Free();
	}
}

Texture * TextureCache::GetFromHash(size_t hash)
{
	if (textures_.find(hash) != textures_.end())
	{
		return &textures_[hash];
	}
	
	debug::Log(SDL_LOG_PRIORITY_CRITICAL, SDL_LOG_CATEGORY_RENDER, "Requested texture was null.");
	return nullptr;
}

Texture * TextureCache::GetFromFile(const std::string & path, bool mips, size_t * hash)
{

	size_t path_hash = std::hash<std::string>{}(path);

	if (textures_.find(path_hash) == textures_.end())
	{
		Texture texture;
		texture.Create(path, mips);
		textures_.insert({ path_hash, std::move(texture) });
	}

	if (hash)
	{
		*hash = path_hash;
	}

	return &textures_[path_hash];
}

Texture * TextureCache::GetFromSurface(SDL_Surface * surface, const std::string &name, bool mips, size_t * hash)
{
	SDL_assert(surface);

	size_t path_hash = std::hash<std::string>{}(name);

	if (textures_.find(path_hash) == textures_.end())
	{
		Texture texture;
		texture.Create(surface, mips);
		textures_.insert({ path_hash, std::move(texture) });
	}
	else
	{
		SDL_assert(false); // We should never ever get the same hash based on name
	}

	if (hash)
	{
		*hash = path_hash;
	}

	return &textures_[path_hash];
}