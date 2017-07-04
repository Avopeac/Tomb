#include "texture.h"

#include <functional>

#include "SDL.h"
#include "SDL_image.h"

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
		unit_ = other.unit_;
		other.id_ = 0;
		other.unit_ = 0;
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

	SDL_Surface * surface = IMG_LoadPNG_RW(rw);

	Create(surface, mips);

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
		internal_format = GL_RGB8;
	}
	else if (pixel_format.BytesPerPixel == 4)
	{
		internal_format = GL_RGBA8;
	}

	SDL_Surface * converted_surface = SDL_ConvertSurface(surface, &pixel_format, 0);

	glBindTexture(GL_TEXTURE_2D, id_);
	glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, converted_surface->w, converted_surface->h);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, converted_surface->w, converted_surface->h, 
		GL_RGBA, GL_UNSIGNED_BYTE, converted_surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (mips)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_FreeSurface(surface);
}

void Texture::Free()
{
	glDeleteTextures(1, &id_);
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

Texture & TextureCache::GetFromHash(size_t hash)
{
	SDL_assert(textures_.find(hash) != textures_.end());

	return textures_[hash];
}

Texture &TextureCache::GetFromFile(size_t &hash, const std::string & path)
{

	hash = std::hash<std::string>{}(path);

	if (textures_.find(hash) == textures_.end())
	{
		Texture texture;
		texture.Create(path, true);
		textures_.insert({ hash, std::move(texture) });
	}

	return textures_[hash];

}

Texture &TextureCache::GetFromSurface(size_t &hash, SDL_Surface * surface, const std::string &name)
{
	hash = std::hash<std::string>{}(name);

	if (textures_.find(hash) == textures_.end())
	{
		Texture texture;
		texture.Create(surface, true);
		textures_.insert({ hash, std::move(texture) });
	}
	else
	{
		SDL_assert(false); // We should never ever get the same hash based on name
	}

	return textures_[hash];
}