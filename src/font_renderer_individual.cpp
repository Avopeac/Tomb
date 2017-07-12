#include "font_renderer_individual.h"

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

#include "timing.h"
#include "logger.h"

using namespace graphics;

FontRendererIndividual::FontRendererIndividual(GraphicsBase & graphics_base, ProgramCache & program_cache,
	TextureCache & texture_cache, SamplerCache & sampler_cache, BlendCache & blend_cache) : 
	graphics_base_(graphics_base), program_cache_(program_cache), 
	texture_cache_(texture_cache), sampler_cache_(sampler_cache),
	blend_cache_(blend_cache)
{
	GenerateGlyphTextures_();

	CreateObjects_(graphics_base_.quad_vertices, sizeof(graphics_base_.quad_vertices) / sizeof(graphics_base_.quad_vertices[0]),
		graphics_base_.quad_indices, sizeof(graphics_base_.quad_indices) / sizeof(graphics_base_.quad_indices[0]));

	vertex_program_ = program_cache_.CompileFromFile(GL_VERTEX_SHADER, "assets/shaders/default_font_individual.vert");
	fragment_program_ = program_cache_.CompileFromFile(GL_FRAGMENT_SHADER, "assets/shaders/default_font_individual.frag");

	pipeline_.SetStages(vertex_program_);
	pipeline_.SetStages(fragment_program_);
}

FontRendererIndividual::~FontRendererIndividual()
{
	DeleteObjects_();
}

void FontRendererIndividual::Push(const std::string & string, const glm::vec4 & color, 
	const glm::vec2 & position, const glm::vec2 & scale)
{

	batch_.push_back(FontRenderInstanceIndiviual());

	auto &text = batch_.back();
	text.color = color;
	text.position = position;
	text.scale = scale;
	for (char c : string)
	{
		text.glyphs.push_back(&glyphs_[c]);
	}
}

void FontRendererIndividual::Draw()
{
	size_t blend_hash;
	blend_cache_.GetFromParameters(blend_hash, BlendMode::SrcAlpha, BlendMode::OneMinusSrcAlpha).Set();

	size_t sampler_hash;
	sampler_cache_.GetFromParameters(sampler_hash, MagnificationFiltering::Nearest, MinificationFiltering::Nearest,
		Wrapping::ClampToEdge, Wrapping::ClampToEdge).Bind(0);

	glBindVertexArray(vertex_array_);

	pipeline_.Bind();

	glProgramUniform1f(fragment_program_.id, glGetUniformLocation(vertex_program_.id, "u_time"), static_cast<float>(util::GetSeconds()));
	glProgramUniform1f(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_time"), static_cast<float>(util::GetSeconds()));
	glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_viewproj"), 1,
		GL_FALSE, glm::value_ptr(graphics_base_.GetViewProjection()));

	for (auto &text : batch_)
	{
		glProgramUniform4fv(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_color"), 1,
			glm::value_ptr(text.color));

		glm::vec2 glyph_position = text.position;

		for (int i = 0; i < text.glyphs.size(); ++i)
		{
			auto glyph = text.glyphs[i];

			glyph_position.x += glyph->min_x;

			glm::vec2 glyph_size = 0.5f * glm::vec2(glyph->texture_w, glyph->texture_h); 

			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, glyph_textures_[glyph->texture_index]);
			glProgramUniform1i(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_texture"), 0);
			glProgramUniform2fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_position"), 1,
				glm::value_ptr(glyph_position));
			glProgramUniform2fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_size"), 1,
				glm::value_ptr(glyph_size));

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glyph_position.x += glyph->advance - glyph->min_x;
		}

	}

	sampler_cache_.GetFromHash(sampler_hash).Unbind();

	pipeline_.Unbind();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void FontRendererIndividual::CreateObjects_(const glm::vec2 * const vertices, Uint32 num_vertices, 
	const Uint32 * const indices, Uint32 num_indices)
{
	num_vertices_ = num_vertices;
	num_indices_ = num_indices;

	auto vertex_size = sizeof(vertices[0]);
	auto index_size = sizeof(indices[0]);
	auto vertices_size = num_vertices_ * vertex_size;
	auto indices_size = num_indices_ * index_size;

	glGenBuffers(1, &vertex_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &element_buffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLint attrib_index = 0;
	glGenVertexArrays(1, &vertex_array_);
	glBindVertexArray(vertex_array_);

	// Index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);

	// Vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);

	// Position attribute

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 2, GL_FLOAT, GL_FALSE, vertex_size, 0);
	glVertexAttribDivisor(attrib_index, 0);
	attrib_index++;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void FontRendererIndividual::DeleteObjects_()
{
	glDeleteVertexArrays(1, &vertex_array_);
	glDeleteBuffers(1, &vertex_buffer_);
	glDeleteBuffers(1, &element_buffer_);

	for (int i = 0; i < glyph_textures_.size(); ++i) {
		glDeleteTextures(1, &glyph_textures_[i]);
	}
}

void FontRendererIndividual::GenerateGlyphTextures_()
{
	// Point size in 72DPI
	const int pt_size = 72;

	// TODO: Less hardcoded
	const std::string default_font_path = "assets/fonts/bellefair/bellefair_regular.ttf";

	// Load the font file
	font_ = TTF_OpenFont(default_font_path.c_str(), pt_size);
	if (!font_)
	{
		debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_INPUT, TTF_GetError());
	}

	font_data_.font_height = TTF_FontHeight(font_);
	font_data_.font_ascent = TTF_FontAscent(font_);
	font_data_.font_descent = TTF_FontDescent(font_);
	font_data_.font_line_skip = TTF_FontLineSkip(font_);

	GLint swapbytes, lsbfirst, rowlength, skiprows, skippixels, alignment;

	// Save current pixel store state.
	glGetIntegerv(GL_UNPACK_SWAP_BYTES, &swapbytes);
	glGetIntegerv(GL_UNPACK_LSB_FIRST, &lsbfirst);
	glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowlength);
	glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skiprows);
	glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skippixels);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);

	// Set desired pixel store state.
	glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glyph_textures_.resize(characters_.size());

	// White base color, easily tintable
	SDL_Color white = { 1, 1, 1, 1 };
	for (uint8_t i = 0; i < glyph_textures_.size(); ++i)
	{
		// Create surface for glyph with high quality
		SDL_Surface * surface = TTF_RenderGlyph_Blended(font_, characters_[i], white);
		if (surface)
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

			surface = SDL_ConvertSurface(surface, &pixel_format, 0);
			glGenTextures(1, &glyph_textures_[i]);
			glBindTexture(GL_TEXTURE_2D, glyph_textures_[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h,
				0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
			glGenerateMipmap(GL_TEXTURE_2D);

			int min_x = 0;
			int min_y = 0;
			int max_x = 0;
			int max_y = 0;
			int advance = 0;

			// TODO: Check for errors
			TTF_GlyphMetrics(font_, characters_[i], &min_x, &max_x, &min_y, &max_y, &advance);

			// Insert glyph
			FontGlyphIndividual glyph;
			glyph.character = characters_[i];
			glyph.texture_index = i;
			glyph.min_x = min_x;
			glyph.max_x = max_x;
			glyph.min_y = min_y;
			glyph.max_y = max_y;
			glyph.texture_w = surface->w;
			glyph.texture_h = surface->h;
			glyph.advance = advance;
			glyphs_.insert({ characters_[i], glyph });

			SDL_FreeSurface(surface);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// Set back old pixel store state.
	glPixelStorei(GL_UNPACK_SWAP_BYTES, swapbytes);
	glPixelStorei(GL_UNPACK_LSB_FIRST, lsbfirst);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, rowlength);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, skiprows);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, skippixels);
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
}
