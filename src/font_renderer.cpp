#include "font_renderer.h"

#include "SDL.h"
#include "SDL_ttf.h"

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

	vertex_program_ = program_cache_.CompileFromFile(GL_VERTEX_SHADER, "assets/shaders/default_font.vert");
	fragment_program_ = program_cache_.CompileFromFile(GL_FRAGMENT_SHADER, "assets/shaders/default_font.frag");

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

	for (size_t i = 0; i < string.length(); ++i)
	{
		char character = string[i];

		int kerning = i > 0 ? TTF_GetFontKerningSizeGlyphs(font_, string[i - 1], character) : 0;

		FontGlyph &glyph = glyphs_[character];

		// TODO: Offset each character position

		RenderCharacterInstance char_instance;
		
		char_instance.positions[0][0] = glyph.min_x;
		char_instance.positions[0][1] = glyph.max_x;
		char_instance.positions[0][2] = glyph.min_y;
		char_instance.positions[0][3] = glyph.max_y;

		char_instance.positions[1][0] = position.x;
		char_instance.positions[1][1] = position.y;
		char_instance.positions[1][2] = scale.x;
		char_instance.positions[1][3] = scale.x;

		char_instance.positions[2][0] = color.x;
		char_instance.positions[2][1] = color.x;
		char_instance.positions[2][2] = color.x;
		char_instance.positions[2][3] = color.x;

		char_instance.positions[3][0] = kerning;
		char_instance.positions[3][1] = glyph.advance;
		char_instance.positions[3][2] = glyph.texture_array_index;

		text_instance.render_chars.push_back(char_instance);
	}

	render_texts_.push_back(text_instance);
}

void FontRenderer::Draw()
{

	glBindVertexArray(vertex_array_);

	pipeline_.Bind();

	size_t hash;
	blend_cache_.GetFromParameters(hash, BlendMode::SrcAlpha, BlendMode::OneMinusSrcAlpha);

	glBindTextureUnit(0, texture_array_);
	glProgramUniform1i(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_texture"), 0);
	glProgramUniform1f(fragment_program_.id, glGetUniformLocation(vertex_program_.id, "u_time"), static_cast<float>(util::GetSeconds()));
	glProgramUniform1f(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_time"), static_cast<float>(util::GetSeconds()));
	glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_viewproj"), 1,
		GL_FALSE, glm::value_ptr(graphics_base_.GetViewProjection()));


	for (auto &text : render_texts_)
	{

		glProgramUniform4fv(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_color"),
			1, glm::value_ptr(text.color));

		// Re-upload subdata for instance buffer
		glNamedBufferSubData(instance_buffer_, 0,
			static_cast<GLsizeiptr>(text.render_chars.size() * sizeof(RenderCharacterInstance)),
			&text.render_chars[0]);

		glDrawElementsInstanced(GL_TRIANGLES,
			static_cast<GLsizei>(num_indices_), GL_UNSIGNED_INT, 0,
			static_cast<GLsizei>(text.render_chars.size()));
	}

	pipeline_.Unbind();
}

void FontRenderer::GenerateGlyphTextures()
{

	// Point size in 72DPI
	const int pt_size = 16;

	// TODO: Less hardcoded
	const std::string default_font_path = "assets/fonts/bellefair/bellefair_regular.ttf";

	// Load the font file
	font_ = TTF_OpenFont(default_font_path.c_str(), pt_size);
	if (!font_)
	{
		debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_INPUT, TTF_GetError());
	}

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

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture_array_);
	glTextureParameteri(texture_array_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture_array_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture_array_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture_array_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture_array_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureStorage3D(texture_array_, 1, GL_RGBA8, 64, 64, static_cast<GLsizei>(characters_.size()));

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
			glTextureSubImage3D(texture_array_, 0, 0, 0, i, surface->w, surface->h,
				1, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

			int min_x;
			int min_y;
			int max_x;
			int max_y;
			int advance;

			// TODO: Check for errors
			TTF_GlyphMetrics(font_, character, &min_x, &max_x, &min_y, &max_y, &advance);

			// Insert glyph
			FontGlyph glyph;
			glyph.texture_array_index = i;
			glyph.min_x = min_x;
			glyph.max_x = max_x;
			glyph.min_y = min_y;
			glyph.max_y = max_y;
			glyph.advance = advance;
			glyphs_.insert({ character, glyph });

			SDL_FreeSurface(surface);
		}
	}


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

	glCreateBuffers(1, &vertex_buffer_);
	glCreateBuffers(1, &element_buffer_);
	glCreateBuffers(1, &instance_buffer_);

	glNamedBufferData(vertex_buffer_, vertices_size, vertices, GL_STATIC_DRAW);
	glNamedBufferData(element_buffer_, indices_size, indices, GL_STATIC_DRAW);
	glNamedBufferData(instance_buffer_, MAX_CHARS_PER_FONT_RENDER_INSTANCE * instance_size, nullptr, GL_DYNAMIC_DRAW);

	glCreateVertexArrays(1, &vertex_array_);

	glVertexArrayElementBuffer(vertex_array_, element_buffer_);
	glVertexArrayVertexBuffer(vertex_array_, 0, vertex_buffer_, 0, static_cast<GLsizei>(vertex_size));
	glVertexArrayVertexBuffer(vertex_array_, 1, instance_buffer_, 0, static_cast<GLsizei>(instance_size));

	GLint attrib_index = 0;

	// Position attribute
	glEnableVertexArrayAttrib(vertex_array_, attrib_index);
	glVertexArrayAttribBinding(vertex_array_, attrib_index, 0);
	glVertexArrayAttribFormat(vertex_array_, attrib_index, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayBindingDivisor(vertex_array_, attrib_index++, 0);

	// Transform attribute
	for (int i = 0; i < 4; ++i)
	{
		glEnableVertexArrayAttrib(vertex_array_, attrib_index);
		glVertexArrayAttribBinding(vertex_array_, attrib_index, 1);
		glVertexArrayAttribFormat(vertex_array_, attrib_index, 4, GL_FLOAT, GL_FALSE, i * sizeof(glm::vec4));
		glVertexArrayBindingDivisor(vertex_array_, attrib_index++, 1);
	}

	// Texture index attribute
	glEnableVertexArrayAttrib(vertex_array_, attrib_index);
	glVertexArrayAttribBinding(vertex_array_, attrib_index, 1);
	glVertexArrayAttribFormat(vertex_array_, attrib_index, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(RenderCharacterInstance, RenderCharacterInstance::texture_array_index));
	glVertexArrayBindingDivisor(vertex_array_, attrib_index++, 1);
}

void FontRenderer::DeleteObjects()
{
	glDeleteVertexArrays(1, &vertex_array_);
	glDeleteBuffers(1, &vertex_buffer_);
	glDeleteBuffers(1, &element_buffer_);
	glDeleteBuffers(1, &instance_buffer_);
	glDeleteTextures(1, &texture_array_);
}
