#include "post_processing.h"

#include "renderer.h"

using namespace graphics;

const GraphicsBase * PostProcessEffect::graphics_base_ = nullptr;

PostProcessing::PostProcessing(GraphicsBase &graphics_base)
	: graphics_base_(graphics_base)
{
	PostProcessEffect::Init(graphics_base_);
} 

PostProcessing::~PostProcessing()
{
}

void PostProcessing::Add(std::unique_ptr<PostProcessEffect> effect)
{
	effects_.push_back(std::move(effect));
	effects_.back()->Init();
}

void PostProcessing::Process()
{
	for (auto &effect : effects_)
	{
		effect->Apply();
	}
}

GLuint PostProcessEffect::vao_ = 0;
GLuint PostProcessEffect::vbo_ = 0;
GLuint PostProcessEffect::ebo_ = 0; 

void PostProcessEffect::Init(const GraphicsBase &graphics_base)
{

	graphics_base_ = &graphics_base;

	size_t num_vertices = graphics_base_->GetNumQuadVertices();
	size_t num_indices = graphics_base_->GetNumQuadIndices();

	auto vertex_size = sizeof(graphics_base_->GetQuadVertices()[0]);
	auto index_size = sizeof(graphics_base_->GetQuadIndices()[0]);
	auto vertices_size = num_vertices * vertex_size;
	auto indices_size = num_indices * index_size;

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, graphics_base_->GetQuadVertices(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ebo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, graphics_base_->GetQuadIndices(), GL_STATIC_DRAW);
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
