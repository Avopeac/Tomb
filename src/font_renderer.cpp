#include "font_renderer.h"

#include "SDL.h"
#include "SDL_ttf.h"

#include "logger.h"

using namespace graphics;

FontRenderer::FontRenderer(GraphicsBase & graphics_base, ProgramCache & program_cache,
	TextureCache & texture_cache, SamplerCache &sampler_cache, BlendCache &blend_cache)
{

	TTF_Font * font = TTF_OpenFont("assets/fonts/bellefair/bellefair_regular.ttf", 16);
	if (!font)
	{
		debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_INPUT, TTF_GetError());
	}

	SDL_Color white = { 1, 1, 1, 1 };

	for (char character : characters_)
	{
		
		SDL_Surface * surface = TTF_RenderGlyph_Blended(font, character, white);
		if (surface)
		{
			size_t hash;
			int min_x, min_y, max_x, max_y, advance;
			texture_cache.GetFromSurface(hash, surface);

			// TODO: Check for errors
			TTF_GlyphMetrics(font, character, &min_x, &max_x, &min_y, &max_y, &advance);

			FontGlyph glyph;
			glyph.texture_hash = hash;
			glyph.min_x = min_x;
			glyph.max_y = max_y;
			glyph.advance = advance;
			glyphs_.insert({ character, glyph });
			
		}
	}
}

FontRenderer::~FontRenderer()
{
}

void FontRenderer::Push(const std::string & string)
{

}

void FontRenderer::Draw()
{

}
