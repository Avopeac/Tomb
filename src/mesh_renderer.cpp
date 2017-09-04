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
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Push(size_t mesh_hash, size_t texture_hash, const glm::mat4 &model, const glm::vec4 &color)
{
	MeshRenderInstance instance;
	instance.color = color;
	instance.mesh = &mesh_cache_.GetFromHash(mesh_hash);
	instance.model = model;
	instance.texture = &texture_cache_.GetFromHash(texture_hash);

	meshes_.push_back(instance);
}

void MeshRenderer::Draw()
{
	pipeline_.Bind();

	glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_proj"), 
		1, GL_FALSE, glm::value_ptr(graphics_base_.GetPerspProj()));

	glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_view"), 
		1, GL_FALSE, glm::value_ptr(graphics_base_.GetPerspView())); 

	glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_vp"),
		1, GL_FALSE, glm::value_ptr(graphics_base_.GetPerspViewProj()));

	for (int i = 0; i < meshes_.size(); ++i)
	{
		auto &instance = meshes_[i];

		glm::mat4 mvp = graphics_base_.GetPerspViewProj() * instance.model;

		glm::mat4 normal_matrix = glm::inverse(glm::transpose(graphics_base_.GetPerspView() * instance.model));

		glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_mvp"),
			1, GL_FALSE, glm::value_ptr(mvp));

		glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_model"),
			1, GL_FALSE, glm::value_ptr(instance.model));

		glProgramUniformMatrix4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_normal"),
			1, GL_FALSE, glm::value_ptr(normal_matrix));

		glProgramUniform4fv(vertex_program_.id, glGetUniformLocation(vertex_program_.id, "u_color"),
			1, glm::value_ptr(instance.color));

		glBindVertexArray(instance.mesh->vao);
		size_t index_offset = 0;
		for (Uint32 j = 0; j < instance.mesh->num_meshes; ++j)
		{
			glDrawElements(GL_TRIANGLES, (GLsizei)instance.mesh->num_indices[j], GL_UNSIGNED_INT, (void*)index_offset);
			index_offset += instance.mesh->num_indices[j] * sizeof(Uint32); 
		}
	}
	glBindVertexArray(0);

	pipeline_.Unbind();

	meshes_.clear();
}
