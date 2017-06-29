#pragma once

#include <vector>

#include <unordered_map>

#include "graphics.h"
#include "blend_mode.h"
#include "sprite.h"
#include "shader.h"
#include "sampler.h"
#include "texture.h"

namespace graphics
{

	struct SpriteBatchInstance
	{
		glm::mat4 transform;
		Uint32 layer;
	};

	struct SpriteBatch
	{
		size_t texture_hash;
		size_t blend_hash;
		size_t sampler_hash;
		size_t shape_hash;
		std::vector<SpriteBatchInstance> instances;
	};

	enum class SpriteShape
	{
		Rectangle = 0,
		FlatHex,
		SharpHex
	};


	struct SpriteRendererBatchObjects
	{
		GLuint vertex_array = 0;
		GLuint vertex_buffer = 0;
		GLuint element_buffer = 0;
		GLuint instance_buffer = 0;
		size_t num_indices = 0;
		size_t num_vertices = 0;
	};

	class SpriteRenderer
	{

		size_t instances_per_batch_;

		Program vertex_program_;

		Program fragment_program_;

		ProgramPipeline pipeline_;

		ProgramCache & program_cache_;

		GraphicsBase & graphics_base_;

		TextureCache & texture_cache_;

		SamplerCache & sampler_cache_;

		BlendCache & blend_cache_;

		std::vector<SpriteBatch> sharp_hex_sprite_batches_;

		std::vector<SpriteBatch> flat_hex_sprite_batches_;

		std::vector<SpriteBatch> rectangular_sprite_batches_;

		SpriteRendererBatchObjects sharp_hex_objects;

		SpriteRendererBatchObjects flat_hex_objects;

		SpriteRendererBatchObjects rectangular_objects;

	public:

		SpriteRenderer(size_t instances_per_batch, GraphicsBase & graphics_base, ProgramCache & program_cache,
			TextureCache & texture_cache, SamplerCache &sampler_cache, BlendCache &blend_cache);

		~SpriteRenderer();

		SpriteRenderer(const SpriteRenderer &) = delete;

		SpriteRenderer(const SpriteRenderer &&) = delete;

		SpriteRenderer &operator=(const SpriteRenderer &) = delete;

		SpriteRenderer &operator=(SpriteRenderer &&) = delete;

		void Push(const Sprite & sprite, SpriteShape shape, const std::string &texture_path, BlendMode src_color_blend,
			BlendMode dst_color_blend, BlendMode src_alpha_blend, BlendMode dst_alpha_blend,
			MagnificationFiltering mag, MinificationFiltering min,
			Wrapping s, Wrapping t);

		void Draw();

	private:

		void PushToBatchObject_(std::vector<SpriteBatch>& batches,
			SpriteBatchInstance &instance,
			size_t sampler_hash,
			size_t blend_hash,
			size_t texture_hash);

		void CreateBatchObject_(SpriteRendererBatchObjects &objects, const glm::vec2 * const vertices, 
			Uint32 num_vertices, const Uint32 * const indices, Uint32 num_indices);

		void DeleteBatchObject_(SpriteRendererBatchObjects &objects);

		void DrawBatchObject_(SpriteRendererBatchObjects &objects, std::vector<SpriteBatch> &batch);


	};
}
