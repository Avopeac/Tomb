#pragma once

#include <string>
#include <unordered_map>

#include "SDL_ttf.h"

#include "graphics.h"
#include "shader.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"

namespace graphics
{

	#define MAX_CHARS_PER_FONT_RENDER_INSTANCE 64

	struct FontGlyph
	{
		size_t texture_array_index;
		size_t texture_w;
		size_t texture_h;
		size_t min_x;
		size_t max_x;
		size_t min_y;
		size_t max_y;
		size_t advance;
	};
	
	struct RenderCharacterInstance
	{
		glm::mat4 positions;
		Uint32 texture_array_index;
	};

	struct RenderTextInstance
	{
		glm::vec4 color;
		std::vector<RenderCharacterInstance> render_chars;
	};

	class FontRenderer
	{
		const std::string characters_ = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz!?0123456789.=,:-;(%){#}[&]<*>";

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

		GLuint instance_buffer_;

		GLuint texture_array_;

		ProgramPipeline pipeline_;

		Program vertex_program_;

		Program fragment_program_;

		std::vector<RenderTextInstance> render_texts_;

		std::unordered_map<char, FontGlyph> glyphs_;

		TTF_Font * font_;

	public:

		FontRenderer(GraphicsBase & graphics_base, ProgramCache & program_cache,
			TextureCache & texture_cache, SamplerCache &sampler_cache, BlendCache &blend_cache);

		~FontRenderer();

		void Push(const std::string & string, const glm::ivec4 &color,
			const glm::vec2 &position, const glm::vec2 &scale);

		void Draw();

	private:

		void GenerateGlyphTextures();

		void CreateObjects(const glm::vec2 * const vertices,
			Uint32 num_vertices, const Uint32 * const indices, Uint32 num_indices);

		void DeleteObjects();

	};
}
