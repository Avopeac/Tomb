#pragma once

#include <string>
#include <unordered_map>

#include "graphics.h"
#include "shader.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"

namespace graphics
{

	struct FontGlyph
	{
		size_t texture_hash;
		size_t min_x;
		size_t max_y;
		size_t advance;
	};

	class FontRenderer
	{
		const std::string characters_ = "0123456789AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"; // !.#\"\'&%()/{}[]\\=-_:;,<>|*^";

		GLuint vertex_array_;

		GLuint vertex_buffer_;

		GLuint element_buffer_;

		GLuint instance_buffer_;

		std::unordered_map<char, FontGlyph> glyphs_;

	public:

		FontRenderer(GraphicsBase & graphics_base, ProgramCache & program_cache,
			TextureCache & texture_cache, SamplerCache &sampler_cache, BlendCache &blend_cache);

		~FontRenderer();

		void Push(const std::string &string);

		void Draw();
	};
}
