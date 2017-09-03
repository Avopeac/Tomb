#include "cube.h"

#include "timing.h"

using namespace graphics;

Cube::Cube(const GraphicsBase & graphics_base,
	TextureCache & texture_cache,
	SamplerCache & sampler_cache,
	FrameBufferCache & frame_buffer_cache,
	ProgramCache & program_cache,
	BlendCache & blend_cache) :
	graphics_base_(graphics_base),
	texture_cache_(texture_cache),
	sampler_cache_(sampler_cache),
	frame_buffer_cache_(frame_buffer_cache),
	program_cache_(program_cache),
	blend_cache_(blend_cache)
{
}

Cube::~Cube()
{
}

void Cube::Init()
{

	view_ = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	proj_ = glm::perspective(glm::radians(60.0f), 
		(float)graphics_base_.GetBackbufferWidth() / graphics_base_.GetBackbufferHeight(), 
		0.01f, 1000.0f);

	/*GLuint indices[] = {
		0, 1, 2, 2, 1, 3,
	};

	glm::vec3 vertices[] = {
		{ -1, -1, 0, },
		{ 1, -1, 0, },
		{ -1, 1, 0, },
		{ 1, 1, 0, },
	};*/

	Uint32 indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};

	glm::vec3 vertices[] = {
		// front
		{-1.0, -1.0,  1.0,},
	    {1.0, -1.0,  1.0, },
	    {1.0,  1.0,  1.0,},
		{-1.0,  1.0,  1.0},
		// back
		{-1.0, -1.0, -1.0,},
		{1.0, -1.0, -1.0,},
		{1.0,  1.0, -1.0,},
		{-1.0,  1.0, -1.0,},
	};

	glGenBuffers(1, &ebo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * num_indices_, &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * num_vertices_, &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(glm::vec3), (void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	size_t h;
	vertex_shader_ = &program_cache_.GetFromFile("cube.vert", h, GL_VERTEX_SHADER, "assets/shaders/cube.vert");
	fragment_shader_ = &program_cache_.GetFromFile("cube.frag", h, GL_FRAGMENT_SHADER , "assets/shaders/cube.frag");

	pipeline_.SetStages(*vertex_shader_);
	pipeline_.SetStages(*fragment_shader_);
}

void Cube::Render(float frame_time)
{

	static double angle = 0.0;
	angle += frame_time;

	pipeline_.Bind();

	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5.0)) * 
		glm::rotate(glm::mat4(1), glm::two_pi<float>() * glm::sin(100.0f * (float)angle),
			glm::vec3(0, 1, 0)); 

	glm::mat4 mvp = proj_ * view_ * model;

	glProgramUniformMatrix4fv(vertex_shader_->id,
		glGetUniformLocation(vertex_shader_->id, "u_viewproj"),
		1, GL_FALSE, glm::value_ptr(mvp));

	glBindVertexArray(vao_);
	glDrawElements(GL_TRIANGLES, (GLsizei)num_indices_, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); 
	pipeline_.Unbind();

	//glEnable(GL_DEPTH_TEST);
} 
