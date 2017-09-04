#include "sprite_renderer.h"

#include <algorithm>

#include "GL/glew.h"

#include "glm/gtc/type_ptr.hpp"

#include "timing.h"

#include "logger.h"

using namespace graphics;

SpriteRenderer::SpriteRenderer(size_t instances_per_batch, GraphicsBase & graphics_base, ProgramCache & program_cache,
	TextureCache & texture_cache, SamplerCache &sampler_cache, BlendCache &blend_cache) :
	instances_per_batch_(instances_per_batch),
	graphics_base_(graphics_base),
	program_cache_(program_cache),
	texture_cache_(texture_cache),
	sampler_cache_(sampler_cache),
	blend_cache_(blend_cache) 
{


	CreateBatchObject_(flat_hex_objects,
		graphics_base_.flat_hexagon_vertices, sizeof(graphics_base_.flat_hexagon_vertices) / sizeof(graphics_base_.flat_hexagon_vertices[0]),
		graphics_base_.hexagon_indices, sizeof(graphics_base_.hexagon_indices) / sizeof(graphics_base_.hexagon_indices[0])
	);

	CreateBatchObject_(sharp_hex_objects,
		graphics_base_.sharp_hexagon_vertices, sizeof(graphics_base_.sharp_hexagon_vertices) / sizeof(graphics_base_.sharp_hexagon_vertices[0]),
		graphics_base_.hexagon_indices, sizeof(graphics_base_.hexagon_indices) / sizeof(graphics_base_.hexagon_indices[0])
	);

	CreateBatchObject_(rectangular_objects,
		graphics_base_.quad_vertices, sizeof(graphics_base_.quad_vertices) / sizeof(graphics_base_.quad_vertices[0]),
		graphics_base_.quad_indices, sizeof(graphics_base_.quad_indices) / sizeof(graphics_base_.quad_indices[0])
	);

	size_t v, f;
	vertex_program_ = program_cache_.GetFromFile("default.vert", v, GL_VERTEX_SHADER, "assets/shaders/default.vert");
	fragment_program_ = program_cache_.GetFromFile("default.frag", f, GL_FRAGMENT_SHADER, "assets/shaders/default.frag");

	pipeline_.SetStages(vertex_program_);
	pipeline_.SetStages(fragment_program_);
}

SpriteRenderer::~SpriteRenderer()
{
	DeleteBatchObject_(flat_hex_objects);
	DeleteBatchObject_(sharp_hex_objects);
	DeleteBatchObject_(rectangular_objects);
}

void SpriteRenderer::Push(const Sprite & sprite,
	SpriteShape shape,
	const std::string &texture_path,
	BlendMode src_color_blend,
	BlendMode dst_color_blend,
	BlendMode src_alpha_blend,
	BlendMode dst_alpha_blend,
	MagnificationFiltering mag,
	MinificationFiltering min,
	Wrapping s,
	Wrapping t)
{

	SpriteBatchInstance instance;
	instance.layer = static_cast<Uint32>(sprite.GetLayer());
	instance.transform = sprite.GetTransform();

	size_t sampler_hash, blend_hash, texture_hash;
	texture_cache_.GetFromFile(texture_hash, texture_path);
	sampler_cache_.GetFromParameters(sampler_hash, mag, min, s, t);
	blend_cache_.GetFromParameters(blend_hash, src_color_blend, src_alpha_blend,
		dst_color_blend, dst_alpha_blend);

	switch (shape)
	{
		case graphics::SpriteShape::SharpHex:
			PushToBatchObject_(sharp_hex_sprite_batches_, instance,
				sampler_hash, blend_hash, texture_hash);
			break;
		case graphics::SpriteShape::Rectangle:
			PushToBatchObject_(rectangular_sprite_batches_, instance,
				sampler_hash, blend_hash, texture_hash);
			break;
		case graphics::SpriteShape::FlatHex:
			PushToBatchObject_(flat_hex_sprite_batches_, instance,
				sampler_hash, blend_hash, texture_hash);
			break;
		default:
			break;
	}
}

void SpriteRenderer::Draw()
{

	pipeline_.Bind();

	// Set standard uniforms
	// TODO: Cache uniform locations at program creation
	glProgramUniform1i(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_texture"), 0);
	glProgramUniform1f(fragment_program_.id, glGetUniformLocation(vertex_program_.id, "u_time"), static_cast<float>(util::GetSeconds()));
	glProgramUniform1f(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_time"), static_cast<float>(util::GetSeconds()));

	int x, y;
	SDL_GetMouseState(&x, &y);
	float normalized_x = 2.0f * ((float)x / graphics_base_.GetBackbufferWidth()) - 1.0f;
	float normalized_y = 2.0f * ((float)y / graphics_base_.GetBackbufferHeight()) - 1.0f;

	//std::string coordinate = std::to_string(normalized_x);
	//coordinate.append(" " + std::to_string(normalized_y));
	//debug::Log(SDL_LOG_PRIORITY_DEBUG, SDL_LOG_CATEGORY_INPUT, coordinate.c_str());
	glProgramUniform2f(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_mouse"), normalized_x, normalized_y);

	glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_viewproj"), 1,
		GL_FALSE, glm::value_ptr(graphics_base_.GetOrthoViewProj()));

	DrawBatchObject_(sharp_hex_objects, sharp_hex_sprite_batches_);
	DrawBatchObject_(flat_hex_objects, flat_hex_sprite_batches_);
	DrawBatchObject_(rectangular_objects, rectangular_sprite_batches_);

	pipeline_.Unbind();

	//sharp_hex_sprite_batches_.clear();
	//flat_hex_sprite_batches_.clear();
	//rectangular_sprite_batches_.clear();
}

void SpriteRenderer::PushToBatchObject_(std::vector<SpriteBatch>& batches,
	SpriteBatchInstance &instance,
	size_t sampler_hash,
	size_t blend_hash,
	size_t texture_hash)
{
	if (batches.empty() ||
		texture_hash != batches.back().texture_hash ||
		blend_hash != batches.back().blend_hash ||
		sampler_hash != batches.back().sampler_hash)
	{
		// Add sprite data to new sprite batch
		SpriteBatch batch;
		batch.texture_hash = texture_hash;
		batch.blend_hash = blend_hash;
		batch.sampler_hash = sampler_hash;
		batch.instances.push_back(instance);
		batches.push_back(batch);
	}
	else
	{
		// Add sprite data to old sprite batch
		batches.back().instances.push_back(instance);
	}
}

void SpriteRenderer::CreateBatchObject_(SpriteRendererBatchObjects &objects, const glm::vec2 * const vertices,
	Uint32 num_vertices, const Uint32 * const indices, Uint32 num_indices)
{

	objects.num_vertices = num_vertices;
	objects.num_indices = num_indices;

	auto vertex_size = sizeof(vertices[0]);
	auto index_size = sizeof(indices[0]);
	auto vertices_size = objects.num_vertices * vertex_size;
	auto indices_size = objects.num_indices * index_size;
	auto instance_size = sizeof(SpriteBatchInstance);

	glGenBuffers(1, &objects.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, objects.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &objects.instance_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, objects.instance_buffer);
	glBufferData(GL_ARRAY_BUFFER, instances_per_batch_ * instance_size, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &objects.element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects.element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLint attrib_index = 0;
	glGenVertexArrays(1, &objects.vertex_array);
	glBindVertexArray(objects.vertex_array);

	// Index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects.element_buffer);

	// Vertex data
	glBindBuffer(GL_ARRAY_BUFFER, objects.vertex_buffer);

	// Position attribute

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 2, GL_FLOAT, GL_FALSE, (GLsizei)vertex_size, 0);
	glVertexAttribDivisor(attrib_index, 0);
	attrib_index++;

	// Instance data
	glBindBuffer(GL_ARRAY_BUFFER, objects.instance_buffer);

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

	// Layer attribute

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 1, GL_UNSIGNED_INT, GL_FALSE, (GLsizei)instance_size, (const void *)(4 * sizeof(glm::vec4)));
	glVertexBindingDivisor(attrib_index, 1);
	attrib_index++;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void graphics::SpriteRenderer::DeleteBatchObject_(SpriteRendererBatchObjects & objects)
{
	glDeleteVertexArrays(1, &objects.vertex_array);
	glDeleteBuffers(1, &objects.vertex_buffer);
	glDeleteBuffers(1, &objects.element_buffer);
	glDeleteBuffers(1, &objects.instance_buffer);
}

void SpriteRenderer::DrawBatchObject_(SpriteRendererBatchObjects & objects, std::vector<SpriteBatch> & batches)
{

	glBindVertexArray(objects.vertex_array);

	for (auto &batch : batches)
	{
		// Sort by layer within batch
		std::sort(batch.instances.begin(), batch.instances.end(), [](const SpriteBatchInstance &a, const SpriteBatchInstance &b)
		{
			return a.layer < b.layer;
		});

		// Set blend mode for batch
		blend_cache_.GetFromHash(batch.blend_hash).Set();

		// Set sampler and texture if present
		if (batch.sampler_hash && batch.texture_hash)
		{
			sampler_cache_.GetFromHash(batch.sampler_hash).Bind(0);
			texture_cache_.GetFromHash(batch.texture_hash).Bind(0);
		}

		// Re-upload subdata for instance buffer
		glBindBuffer(GL_ARRAY_BUFFER, objects.instance_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizei)(batch.instances.size() * sizeof(SpriteBatchInstance)),
			&batch.instances[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Draw all sprites for the batch
		glDrawElementsInstanced(GL_TRIANGLES,
			(GLsizei)objects.num_indices, GL_UNSIGNED_INT, 0,
			(GLsizei)batch.instances.size());

		batch.instances.clear();
	}

	glBindVertexArray(0);
}
