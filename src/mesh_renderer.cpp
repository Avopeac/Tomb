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
	mesh_cache_(mesh_cache),
	camera_(graphics_base, "Debug camera",
		glm::vec4(0, 0, 0, 1), glm::vec4(0, 0, -1, 0), glm::vec4(0, 1, 0, 0))
{
	size_t h;
	vertex_program_ = program_cache_.GetFromFile("deferred.vert", h, GL_VERTEX_SHADER, "assets/shaders/deferred.vert");
	fragment_program_ = program_cache_.GetFromFile("deferred.frag", h, GL_FRAGMENT_SHADER, "assets/shaders/deferred.frag");
	pipeline_.SetStages(vertex_program_);
	pipeline_.SetStages(fragment_program_);
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Push(size_t mesh_hash, size_t texture_hash, const glm::mat4 &model, const std::function<void(Program *, Program *)> &update)
{
	MeshRenderInstance instance;
	instance.mesh = &mesh_cache_.GetFromHash(mesh_hash);
	instance.texture = &texture_cache_.GetFromHash(texture_hash);
	instance.model = model;
	instance.update = update;
	meshes_.push_back(instance);
}

void MeshRenderer::Draw(float delta_time)
{

	camera_.Update(delta_time);

	std::sort(meshes_.begin(), meshes_.end());

	pipeline_.Bind();
		
	for (int i = 0; i < meshes_.size(); ++i)
	{

		auto &instance = meshes_[i];
		int texture_id = 0;
		glm::mat4 mvp = camera_.GetViewProj() * instance.model;
		glm::mat4 mv = camera_.GetView() * instance.model;
		glm::mat4 normal_matrix = glm::inverse(glm::transpose(camera_.GetView() * instance.model));

		vertex_program_.SetUniform("u_proj", (void*)glm::value_ptr(camera_.GetProj()));
		vertex_program_.SetUniform("u_view", (void*)glm::value_ptr(camera_.GetView()));
		vertex_program_.SetUniform("u_vp", (void*)glm::value_ptr(camera_.GetViewProj()));
		vertex_program_.SetUniform("u_mv", (void *)glm::value_ptr(mv));
		vertex_program_.SetUniform("u_mvp", (void*)glm::value_ptr(mvp));
		vertex_program_.SetUniform("u_model", (void*)glm::value_ptr(instance.model));
		vertex_program_.SetUniform("u_normal", (void*)glm::value_ptr(normal_matrix));
		fragment_program_.SetUniform("u_texture", (void*)&texture_id);

		size_t h;
		auto &sampler = sampler_cache_.GetFromParameters(h, graphics::MagnificationFiltering::Linear,
			graphics::MinificationFiltering::LinearMipmapLinear, graphics::Wrapping::ClampToEdge, graphics::Wrapping::ClampToEdge);

		sampler.Bind(texture_id);
		instance.texture->Bind(texture_id);

		if (instance.update)
		{
			instance.update(&vertex_program_, &fragment_program_);
		}

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
