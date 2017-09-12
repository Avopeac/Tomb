#include "post_processing.h"

using namespace graphics;

const GraphicsBase * PostProcessEffect::graphics_base_ = nullptr;

PostProcessing::PostProcessing(GraphicsBase &graphics_base, TextureCache & texture_cache, 
	ProgramCache & program_cache, 
	SamplerCache & sampler_cache, 
	BlendCache & blend_cache,
	FrameBufferCache & frame_buffer_cache)
	: graphics_base_(graphics_base),
	texture_cache_(texture_cache),
	program_cache_(program_cache),
	sampler_cache_(sampler_cache),
	blend_cache_(blend_cache),
	frame_buffer_cache_(frame_buffer_cache)
{
	PostProcessEffect::Init(graphics_base_);
} 

PostProcessing::~PostProcessing()
{
}

void PostProcessing::Add(std::unique_ptr<PostProcessEffect> effect)
{
	effects_.push_back(std::move(effect));
	effects_.back()->Init(texture_cache_, program_cache_,
		sampler_cache_, blend_cache_, frame_buffer_cache_);
}

void PostProcessing::Process()
{
	for (auto &effect : effects_)
	{
		effect->Apply(texture_cache_, program_cache_, sampler_cache_, 
			blend_cache_, frame_buffer_cache_);
	}
}

GLuint PostProcessEffect::vao_ = 0;
GLuint PostProcessEffect::vbo_ = 0;
GLuint PostProcessEffect::ebo_ = 0; 

void PostProcessEffect::Init(const GraphicsBase &graphics_base)
{

	graphics_base_ = &graphics_base;

	size_t num_vertices = sizeof(graphics_base.quad_vertices) / sizeof(graphics_base.quad_vertices[0]);
	size_t num_indices = sizeof(graphics_base.quad_indices) / sizeof(graphics_base.quad_indices[0]);

	auto vertex_size = sizeof(graphics_base.quad_vertices[0]);
	auto index_size = sizeof(graphics_base.quad_indices[0]);
	auto vertices_size = num_vertices * vertex_size;
	auto indices_size = num_indices * index_size;

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, graphics_base.quad_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ebo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, graphics_base.quad_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLint attrib_index = 0;
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	// Index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

	// Vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	// Position attribute
	glEnableVertexAttribArray (0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (GLsizei)vertex_size, 0);
	glVertexAttribDivisor(0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PostProcessEffect::Render()
{
	glBindVertexArray(vao_);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
