#include "texture.h"

#include <functional>

#include "SDL.h"
#include "SDL_image.h"

using namespace graphics;

Texture::Texture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &id_);
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
	glBindTextureUnit(unit, id_);
	unit_ = unit;
}

void Texture::Unbind()
{
	glBindTextureUnit(unit_, id_);
}

void Texture::Create(const std::string & path, bool mips)
{
	SDL_RWops * rw = SDL_RWFromFile(path.c_str(), "rb");

	SDL_assert(rw);

	SDL_Surface * surface = IMG_LoadPNG_RW(rw);

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

	GLint internalFormat;
	if (pixel_format.BytesPerPixel == 3)
	{
		internalFormat = GL_RGB8;
	}
	else if (pixel_format.BytesPerPixel == 4)
	{
		internalFormat = GL_RGBA8;
	}

	SDL_Surface * converted_surface = SDL_ConvertSurface(surface, &pixel_format, 0);

	glTextureStorage2D(id_, 1, internalFormat, converted_surface->w, converted_surface->h);
	glTextureSubImage2D(id_, 0, 0, 0, converted_surface->w, converted_surface->h,
		GL_RGBA, GL_UNSIGNED_BYTE, converted_surface->pixels);
	glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (mips) 
	{
		glGenerateTextureMipmap(id_);
	}

	SDL_FreeSurface(surface);
	SDL_FreeRW(rw);
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