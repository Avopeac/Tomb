#include "texture.h"

#include <functional>

#include "SDL.h"
#include "SDL_image.h"

using namespace graphics;

Sampler::Sampler()
{
	glGenSamplers(1, &id_);
}

Sampler::~Sampler()
{
	glDeleteSamplers(1, &id_);
}

void Sampler::SetFiltering(MagnificationFiltering mag, MinificationFiltering min)
{
	glSamplerParameteri(id_, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min));
	glSamplerParameteri(id_, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(mag));
}

void Sampler::SetWrap(Wrapping s, Wrapping t)
{
	glSamplerParameteri(id_, GL_TEXTURE_WRAP_S, static_cast<GLint>(s));
	glSamplerParameteri(id_, GL_TEXTURE_WRAP_T, static_cast<GLint>(t));
}

void Sampler::Bind(Uint32 unit) 
{
	unit_ = unit;
	glBindSamplers(unit_, 1, &id_);
}

void Sampler::Unbind()
{
	glBindSamplers(unit_, 0, 0);
}

TextureCache::TextureCache()
{
}

TextureCache::~TextureCache()
{
}

void TextureCache::CreateFromFile(const std::string & path)
{

	SDL_RWops * rw = SDL_RWFromFile(path.c_str(), "rb");

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
	if (pixel_format.BytesPerPixel == 3) {
		internalFormat = GL_RGB8;
	}
	else if (pixel_format.BytesPerPixel == 4) {
		internalFormat = GL_RGBA8;
	}

	SDL_Surface * converted_surface = SDL_ConvertSurface(surface, &pixel_format, 0);

	Texture texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture.id);
	glTextureStorage2D(texture.id, 1, internalFormat, converted_surface->w, converted_surface->h);
	glTextureSubImage2D(texture.id, 0, 0, 0, converted_surface->w, converted_surface->h, 
		GL_RGBA, GL_UNSIGNED_BYTE, converted_surface->pixels);
	glTextureParameteri(texture.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(texture.id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture.id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture.id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateTextureMipmap(texture.id);

	SDL_FreeSurface(surface);
	SDL_FreeRW(rw);

	textures_.insert({ std::hash<std::string>{}(path), texture });
}

Texture & TextureCache::GetTextureFromPath(const std::string & path)
{
	return textures_[std::hash<std::string>{}(path)];
}

Texture & TextureCache::GetTextureFromHash(size_t hash)
{
	return textures_[hash];
}