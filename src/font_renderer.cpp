#include "font_renderer.h"

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

#include "timing.h"
#include "logger.h"

using namespace graphics;

FontRenderer::FontRenderer(GraphicsBase & graphics_base, ProgramCache & program_cache,
	TextureCache & texture_cache, SamplerCache &sampler_cache, BlendCache &blend_cache) :
	graphics_base_(graphics_base), program_cache_(program_cache), texture_cache_(texture_cache),
	sampler_cache_(sampler_cache), blend_cache_(blend_cache)
{
	GenerateGlyphTextures();

	CreateObjects(graphics_base_.quad_vertices, sizeof(graphics_base_.quad_vertices) / sizeof(graphics_base_.quad_vertices[0]),
		graphics_base_.quad_indices, sizeof(graphics_base_.quad_indices) / sizeof(graphics_base_.quad_indices[0]));

	size_t v, f;
	vertex_program_ = program_cache_.GetFromFile("default_font.vert", v, GL_VERTEX_SHADER, 
		"assets/shaders/default_font.vert");
	fragment_program_ = program_cache_.GetFromFile("default_font.frag", f, GL_FRAGMENT_SHADER, 
		"assets/shaders/default_font.frag");

	pipeline_.SetStages(vertex_program_);
	pipeline_.SetStages(fragment_program_);
}

FontRenderer::~FontRenderer()
{
	DeleteObjects();

	// TODO: This causes access violation
	//TTF_CloseFont(font_);
}

void FontRenderer::Push(const std::string & string, const glm::ivec4 &color, 
	const glm::vec2 &position, const glm::vec2 &scale)
{
	RenderTextInstance text_instance;
	text_instance.color = color;

	int advance_accumulation = 0;

	for (size_t i = 0; i < string.length(); ++i)
	{
		char character = string[i];

		FontGlyph &glyph = glyphs_[character];
		
		RenderCharacterInstance char_instance; 
		
		char_instance.positions[0][0] = (float)glyph.min_x;
		char_instance.positions[0][1] = (float)glyph.max_x;
		char_instance.positions[0][2] = (float)glyph.min_y;
		char_instance.positions[0][3] = (float)glyph.max_y;

		char_instance.positions[1][0] = position.x;
		char_instance.positions[1][1] = position.y;
		char_instance.positions[1][2] = scale.x; 
		char_instance.positions[1][3] = scale.y;

		char_instance.positions[2][0] = (float)font_height_;
		char_instance.positions[2][1] = (float)font_descent_;
		char_instance.positions[2][2] = (float)font_ascent_;
		char_instance.positions[2][3] = (float)font_line_skip_;

		char_instance.positions[3][0] = (float)advance_accumulation;
		char_instance.positions[3][1] = (float)glyph.texture_array_index;
		char_instance.positions[3][2] = (float)glyph.texture_w;
		char_instance.positions[3][3] = (float)glyph.texture_h;
		     
		text_instance.render_chars.push_back(char_instance);

		int kerning = 0;
		if (i > 0)
		{
			kerning = TTF_GetFontKerningSizeGlyphs(font_, (Uint16)string[i - 1], (Uint16)character);
			
		}

		advance_accumulation += kerning > 0 ? kerning : (int)glyph.advance;
	}

	render_texts_.push_back(text_instance);
}

void FontRenderer::Draw()
{
	glBindVertexArray(vertex_array_);

	pipeline_.Bind();

	size_t hash;
	blend_cache_.GetFromParameters(hash, BlendMode::SrcAlpha, BlendMode::OneMinusSrcAlpha);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_);
	glProgramUniform1i(fragment_program_.GetId(), glGetUniformLocation(fragment_program_.GetId(), "u_texture"), 0);
	glProgramUniform1f(fragment_program_.GetId(), glGetUniformLocation(vertex_program_.GetId(), "u_time"), static_cast<float>(util::GetSeconds()));
	glProgramUniform1f(fragment_program_.GetId(), glGetUniformLocation(fragment_program_.GetId(), "u_time"), static_cast<float>(util::GetSeconds()));
	glProgramUniformMatrix4fv(vertex_program_.GetId(), glGetUniformLocation(vertex_program_.GetId(), "u_viewproj"), 1,
		GL_FALSE, glm::value_ptr(graphics_base_.GetOrthoViewProj()));


	for (auto &text : render_texts_)
	{

		glProgramUniform4fv(fragment_program_.GetId(), glGetUniformLocation(fragment_program_.GetId(), "u_color"),
			1, glm::value_ptr(text.color));

		// Re-upload subdata for instance buffer
		glBindBuffer(GL_ARRAY_BUFFER, instance_buffer_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(text.render_chars.size() * sizeof(RenderCharacterInstance)),
			&text.render_chars[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Draw characters
		glDrawElementsInstanced(GL_TRIANGLES,
			(GLsizei)num_indices_, GL_UNSIGNED_INT, 0,
			(GLsizei)text.render_chars.size());
	}

	pipeline_.Unbind();
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glBindVertexArray(0);
}

void FontRenderer::GenerateGlyphTextures()
{ 
 
	// Point size in 72DPI
	const int pt_size = 72;

	// TODO: Less hardcoded
	const std::string default_font_path = "assets/fonts/arial/arial.ttf";

	// Load the font file
	font_ = TTF_OpenFont(default_font_path.c_str(), pt_size);
	if (!font_)
	{
		debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_INPUT, TTF_GetError());
	}

	font_height_ = TTF_FontHeight(font_);
	font_ascent_ = TTF_FontAscent(font_);
	font_descent_ = TTF_FontDescent(font_);
	font_line_skip_ = TTF_FontLineSkip(font_);

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

	glGenTextures(1, &texture_array_);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 128, 128, (GLsizei)characters_.size(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	// White base color, easily tintable
	SDL_Color white = { 1, 1, 1, 1 };
	for (size_t i = 0; i < characters_.size(); ++i)
	{
		char character = characters_[i];

		// Create surface for glyph with high quality
		SDL_Surface * surface = TTF_RenderGlyph_Blended(font_, character, white);
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
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, (GLint)i,
				surface->w, surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

			int min_x = 0;
			int min_y = 0;
			int max_x = 0;
			int max_y = 0;
			int advance = 0;

			// TODO: Check for errors
			TTF_GlyphMetrics(font_, character, &min_x, &max_x, &min_y, &max_y, &advance);

			// Insert glyph
			FontGlyph glyph;
			glyph.texture_array_index = i;
			glyph.min_x = min_x;
			glyph.max_x = max_x;
			glyph.min_y = min_y;
			glyph.max_y = max_y;
			glyph.texture_w = surface->w;
			glyph.texture_h = surface->h;
			glyph.advance = advance; 
			glyphs_.insert({ character, glyph });

			SDL_FreeSurface(surface);
		}
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	// Set back old pixel store state.
	glPixelStorei(GL_UNPACK_SWAP_BYTES, swapbytes);
	glPixelStorei(GL_UNPACK_LSB_FIRST, lsbfirst);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, rowlength);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, skiprows);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, skippixels);
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
}

void FontRenderer::CreateObjects(const glm::vec2 * const vertices,
	Uint32 num_vertices, const Uint32 * const indices, Uint32 num_indices)
{

	num_vertices_ = num_vertices;
	num_indices_ = num_indices;

	auto vertex_size = sizeof(vertices[0]);
	auto index_size = sizeof(indices[0]);
	auto vertices_size = num_vertices_ * vertex_size;
	auto indices_size = num_indices_ * index_size;
	auto instance_size = sizeof(RenderCharacterInstance);

	glGenBuffers(1, &vertex_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &instance_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, instance_buffer_);
	glBufferData(GL_ARRAY_BUFFER, MAX_CHARS_PER_FONT_RENDER_INSTANCE * instance_size, nullptr, GL_DYNAMIC_DRAW);
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
	glVertexAttribPointer(attrib_index, 2, GL_FLOAT, GL_FALSE, (GLsizei)vertex_size, 0);
	glVertexAttribDivisor(attrib_index, 0);
	attrib_index++;

	// Instance data
	glBindBuffer(GL_ARRAY_BUFFER, instance_buffer_);

	// Transform attribute

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 4, GL_FLOAT, GL_FALSE, (GLsizei)instance_size, (const void *)(0 * sizeof(glm::vec4)));
	glVertexAttribDivisor(attrib_index, 1);
	attrib_index++;

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 4, GL_FLOAT, GL_FALSE, (GLsizei)instance_size, (const void *)(1 * sizeof(glm::vec4)));
	glVertexAttribDivisor(attrib_index, 1);
	attrib_index++;

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 4, GL_FLOAT, GL_FALSE, (GLsizei)instance_size, (const void *)(2 * sizeof(glm::vec4)));
	glVertexAttribDivisor(attrib_index, 1);
	attrib_index++;

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 4, GL_FLOAT, GL_FALSE, (GLsizei)instance_size, (const void *)(3 * sizeof(glm::vec4)));
	glVertexAttribDivisor(attrib_index, 1);
	attrib_index++;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void FontRenderer::DeleteObjects()
{
	glDeleteVertexArrays(1, &vertex_array_);
	glDeleteBuffers(1, &vertex_buffer_);
	glDeleteBuffers(1, &element_buffer_);
	glDeleteBuffers(1, &instance_buffer_);
	glDeleteTextures(1, &texture_array_);
}
