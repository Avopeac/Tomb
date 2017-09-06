#include "mesh_renderer.h"

#include <algorithm>

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

void MeshRenderer::Push(size_t mesh_hash, size_t texture_hash, 
	size_t vertex_hash, size_t fragment_hash, 
	const glm::mat4 &model, const std::function<void(Program *, Program *)> &update)
{
	MeshRenderInstance instance;
	instance.mesh = &mesh_cache_.GetFromHash(mesh_hash);
	instance.vertex = &program_cache_.GetFromHash(vertex_hash);
	instance.fragment = &program_cache_.GetFromHash(fragment_hash);
	instance.texture = &texture_cache_.GetFromHash(texture_hash);
	instance.model = model;
	instance.update = update;

	meshes_.push_back(instance);
}

void MeshRenderer::Draw()
{
	std::sort(meshes_.begin(), meshes_.end());

	pipeline_.Bind();
		
	for (int i = 0; i < meshes_.size(); ++i)
	{

		auto &instance = meshes_[i];
		int texture_id = 0;
		glm::mat4 mvp = graphics_base_.GetPerspViewProj() * instance.model;
		glm::mat4 normal_matrix = glm::inverse(glm::transpose(graphics_base_.GetPerspView() * instance.model));

		auto &v = *instance.vertex;
		auto &f = *instance.fragment;
		 
		pipeline_.SetStages(v);
		pipeline_.SetStages(f);

		v.SetUniform("u_proj", (void*)glm::value_ptr(graphics_base_.GetPerspProj()));
		v.SetUniform("u_view", (void*)glm::value_ptr(graphics_base_.GetPerspView()));
		v.SetUniform("u_vp", (void*)glm::value_ptr(graphics_base_.GetPerspViewProj()));
		v.SetUniform("u_mvp", (void*)glm::value_ptr(mvp));
		v.SetUniform("u_model", (void*)glm::value_ptr(instance.model));
		v.SetUniform("u_normal", (void*)glm::value_ptr(normal_matrix));
		f.SetUniform("u_texture", (void*)&texture_id);
		
		instance.texture->Bind(0);

		if (instance.update)
			instance.update(&v, &f);

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
