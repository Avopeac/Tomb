#include "mesh_renderer.h"

using namespace graphics;

MeshRenderer::MeshRenderer(GraphicsBase & graphics_base, 
	ProgramCache & program_cache, 
	TextureCache & texture_cache, 
	SamplerCache & sampler_cache, 
	BlendCache & blend_cache, 
	MeshCache & mesh_cache) :

	graphics_base_(graphics_base),
	program_cache_(program_cache),
	texture_cache_(texture_cache),
	sampler_cache_(sampler_cache),
	blend_cache_(blend_cache),
	mesh_cache_(mesh_cache)
{
	size_t h;
	vertex_program_ = program_cache_.GetFromFile("mesh.vert", h, GL_VERTEX_SHADER, "assets/shaders/mesh.vert");
	fragment_program_ = program_cache_.GetFromFile("mesh.frag", h, GL_FRAGMENT_SHADER, "assets/shaders/mesh.frag");

	pipeline_.SetStages(vertex_program_);
	pipeline_.SetStages(fragment_program_);

	view_ = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	proj_ = glm::perspective(glm::radians(60.0f),
		(float)graphics_base_.GetBackbufferWidth() / graphics_base_.GetBackbufferHeight(),
		0.01f, 1000.0f);
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Push(const std::string & name, const std::string & path)
{
	size_t h;
	meshes_.push_back(&mesh_cache_.GetFromFile(h, name, path));
}

void MeshRenderer::Draw(float frame_time)
{
	static double angle = 0.0;
	angle += frame_time;

	pipeline_.Bind();

	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5.0)) *
		glm::rotate(glm::mat4(1), glm::two_pi<float>() * glm::sin(100.0f * (float)angle),
			glm::vec3(0, 1, 0));

	glm::mat4 mvp = proj_ * view_ * model;
	glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_mvp"),
		1, GL_FALSE, glm::value_ptr(mvp));

	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	for (int i = 0; i < meshes_.size(); ++i)
	{
		auto * mesh = meshes_[i];

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
		//uint32_t * ptr = (uint32_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
		//glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		

		glBindVertexArray(mesh->vao);
		size_t index_offset = 0;
		for (Uint32 j = 0; j < mesh->num_meshes; ++j)
		{
			glDrawElements(GL_TRIANGLES, (GLsizei)mesh->num_indices[j], GL_UNSIGNED_INT, (void*)index_offset);
			//glDrawRangeElements(GL_TRIANGLES, index_offset, index_offset + mesh->num_indices[j], 
				//mesh->num_indices[j], GL_UNSIGNED_INT, (void*)0);
			//glDrawElements(GL_TRIANGLES, mesh->num_indices[j], GL_UNSIGNED_INT, (void*)0);
			index_offset += meshes_[i]->num_indices[j] * sizeof(Uint32); 
			//index_offset += meshes_[i]->num_indices[j];
		}
	}
	glBindVertexArray(0);

	glCullFace(GL_NONE);
	glFrontFace(GL_CCW);

	pipeline_.Unbind();

	
}
