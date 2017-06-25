#include "sprite_renderer.h"

#include <algorithm>

#include "GL/glew.h"

#include "glm/gtc/type_ptr.hpp"

using namespace graphics;

SpriteRenderer::SpriteRenderer(size_t instances_per_batch, GraphicsBase & graphics_base, ProgramCache & program_cache,
	TextureCache & texture_cache) :
	instances_per_batch_(instances_per_batch),
	graphics_base_(graphics_base),
	program_cache_(program_cache),
	texture_cache_(texture_cache)
{

	auto quad_vertex_size = sizeof(graphics_base_.quad_vertices[0]);
	auto quad_index_size = sizeof(graphics_base_.quad_indices[0]);
	auto quad_vertices_size = sizeof(graphics_base_.quad_vertices);
	auto quad_indices_size = sizeof(graphics_base_.quad_indices);
	auto num_vertices = quad_vertices_size / sizeof(graphics_base_.quad_vertices[0]);
	auto num_indices = quad_indices_size / sizeof(graphics_base_.quad_indices[0]);
	auto instance_size = sizeof(SpriteBatchInstance);

	glCreateBuffers(1, &vertex_buffer_);
	glCreateBuffers(1, &element_buffer_);
	glCreateBuffers(1, &instance_buffer_);

	glNamedBufferData(vertex_buffer_, quad_vertices_size, graphics_base_.quad_vertices, GL_STATIC_DRAW);
	glNamedBufferData(element_buffer_, quad_indices_size, graphics_base_.quad_indices, GL_STATIC_DRAW);
	glNamedBufferData(instance_buffer_, instances_per_batch_ * instance_size, nullptr, GL_DYNAMIC_DRAW);

	glCreateVertexArrays(1, &vertex_array_);

	glVertexArrayElementBuffer(vertex_array_, element_buffer_);
	glVertexArrayVertexBuffer(vertex_array_, 0, vertex_buffer_, 0, static_cast<GLsizei>(quad_vertex_size));
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
	
	// Layer attribute
	glEnableVertexArrayAttrib(vertex_array_, attrib_index);
	glVertexArrayAttribBinding(vertex_array_, attrib_index, 1);
	glVertexArrayAttribFormat(vertex_array_, attrib_index, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(SpriteBatchInstance, SpriteBatchInstance::layer));
	glVertexArrayBindingDivisor(vertex_array_, attrib_index++, 1);

	// Animation attribute
	glEnableVertexArrayAttrib(vertex_array_, attrib_index);
	glVertexArrayAttribBinding(vertex_array_, attrib_index, 1);
	glVertexArrayAttribFormat(vertex_array_, attrib_index, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(SpriteBatchInstance, SpriteBatchInstance::animation));
	glVertexArrayBindingDivisor(vertex_array_, attrib_index++, 1);

	vertex_program_ = program_cache_.CompileFromFile(GL_VERTEX_SHADER, "assets/shaders/default.vert");
	fragment_program_ = program_cache_.CompileFromFile(GL_FRAGMENT_SHADER, "assets/shaders/default.frag");

	pipeline_.SetStages(vertex_program_);
	pipeline_.SetStages(fragment_program_);

}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &vertex_array_);
	glDeleteBuffers(1, &vertex_buffer_);
	glDeleteBuffers(1, &element_buffer_);
	glDeleteBuffers(1, &instance_buffer_);
}

void SpriteRenderer::Push(const Sprite & sprite, size_t blend_hash, size_t sampler_hash)
{
	size_t texture_hash = sprite.GetTexture();

	SpriteBatchInstance new_instance;
	new_instance.animation = static_cast<Uint32>(sprite.GetAnimation());
	new_instance.layer = static_cast<Uint32>(sprite.GetLayer());
	new_instance.transform = sprite.GetTransform();

	if (sprite_batches_.empty() ||
		texture_hash != sprite_batches_.back().texture_hash ||
		blend_hash != sprite_batches_.back().blend_hash ||
		sampler_hash != sprite_batches_.back().sampler_hash)
	{
		// Add sprite data to new sprite batch
		SpriteBatch new_sprite_batch;
		new_sprite_batch.texture_hash = texture_hash;
		new_sprite_batch.blend_hash = blend_hash;
		new_sprite_batch.instances.push_back(new_instance);
		sprite_batches_.push_back(new_sprite_batch);
	}
	else
	{
		// Add sprite data to old sprite batch
		sprite_batches_.back().instances.push_back(new_instance);
	}
}

void SpriteRenderer::Draw()
{
	// Set depth test
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_NEVER);

	// Set blend mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(vertex_array_);

	pipeline_.Bind();

	auto num_indices = sizeof(graphics_base_.quad_indices) / sizeof(graphics_base_.quad_indices[0]);

	for (auto &batch : sprite_batches_)
	{
		// Sort by layer within batch
		std::sort(batch.instances.begin(), batch.instances.end(), [](const SpriteBatchInstance &a, const SpriteBatchInstance &b)
		{
			return a.layer < b.layer;
		});

		// Get sampler
		//glBindSampler(0,);

		glBindTextureUnit(0, texture_cache_.GetTextureFromHash(batch.texture_hash).id);

		glProgramUniform1i(fragment_program_.id, glGetUniformLocation(fragment_program_.id, "u_texture"), 0);

		glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_viewproj"), 1,
			GL_FALSE, glm::value_ptr(graphics_base_.GetViewProjection()));

		glNamedBufferSubData(instance_buffer_, 0,
			static_cast<GLsizeiptr>(batch.instances.size() * sizeof(SpriteBatchInstance)),
			&batch.instances[0]);

		glDrawElementsInstanced(GL_TRIANGLES,
			static_cast<GLsizei>(num_indices), GL_UNSIGNED_INT, nullptr,
			static_cast<GLsizei>(batch.instances.size()));
	}

	pipeline_.Unbind();
}
