#pragma once

#include <vector>

#include <unordered_map>

#include "graphics.h"
#include "sprite.h"
#include "shader.h"
#include "sampler.h"
#include "texture.h"

namespace graphics
{

	struct SpriteBatchInstance
	{
		glm::mat4 transform = glm::mat4(1);
		Uint32 layer = 0;
		Uint32 animation = 0;
	};

	struct SpriteBatch
	{
		size_t texture_hash = 0;
		size_t blend_hash = 0;
		size_t sampler_hash = 0;
		std::vector<SpriteBatchInstance> instances;
	};

	class SpriteRenderer
	{

		GLuint num_indices_;

		size_t instances_per_batch_;

		GLuint vertex_array_;

		GLuint vertex_buffer_;

		GLuint element_buffer_;

		GLuint instance_buffer_;

		Program vertex_program_;

		Program fragment_program_;

		ProgramPipeline pipeline_;

		ProgramCache & program_cache_;

		GraphicsBase & graphics_base_;

		TextureCache & texture_cache_;

		SamplerCache & sampler_cache_;

		std::vector<SpriteBatch> sprite_batches_;

	public:

		SpriteRenderer(size_t instances_per_batch, GraphicsBase & graphics_base, ProgramCache & program_cache,
			TextureCache & texture_cache, SamplerCache &sampler_cache);

		~SpriteRenderer();

		SpriteRenderer(const SpriteRenderer &) = delete;

		SpriteRenderer(const SpriteRenderer &&) = delete;

		SpriteRenderer &operator=(const SpriteRenderer &) = delete;

		SpriteRenderer &operator=(SpriteRenderer &&) = delete;

		void Push(const Sprite & sprite, size_t blend_hash, 
			MagnificationFiltering mag, MinificationFiltering min,
			Wrapping s, Wrapping t);

		void Draw();

	};
}
