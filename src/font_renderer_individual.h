#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "SDL_ttf.h"

#include "graphics.h"
#include "shader.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"

namespace graphics
{

	struct FontGlyphIndividual
	{
		char character;
		size_t texture_index;
		size_t texture_w;
		size_t texture_h;
		size_t min_x;
		size_t max_x;
		size_t min_y;
		size_t max_y;
		size_t advance;
	};

	struct FontData
	{
		int font_ascent;
		int font_descent;
		int font_line_skip;
		int font_height;
	};

	struct FontRenderInstanceIndiviual
	{
		glm::vec4 color;
		glm::vec2 position;
		glm::vec2 scale;
		std::vector<FontGlyphIndividual *> glyphs;
	};

	class FontRendererIndividual
	{
		GraphicsBase & graphics_base_;

		ProgramCache & program_cache_;

		TextureCache & texture_cache_;

		SamplerCache & sampler_cache_;

		BlendCache & blend_cache_;

		GLuint num_vertices_;

		GLuint num_indices_;

		GLuint vertex_array_;

		GLuint vertex_buffer_;

		GLuint element_buffer_;

		ProgramPipeline pipeline_;

		Program vertex_program_;

		Program fragment_program_;

		TTF_Font * font_;

		FontData font_data_;

		std::vector<GLuint> glyph_textures_;

		std::unordered_map<char, FontGlyphIndividual> glyphs_;

		std::vector<FontRenderInstanceIndiviual> batch_;

	public:

		FontRendererIndividual(GraphicsBase & graphics_base, ProgramCache & program_cache,
			TextureCache & texture_cache, SamplerCache &sampler_cache, BlendCache &blend_cache);

		~FontRendererIndividual();

		void Push(const std::string & string, const glm::vec4 &color,
			const glm::vec2 &position, const glm::vec2 &scale);

		void Draw();

	private:

		void CreateObjects_(const glm::vec2 * const vertices,
			Uint32 num_vertices, const Uint32 * const indices, Uint32 num_indices);

		void DeleteObjects_();

		void GenerateGlyphTextures_();
	};
}
