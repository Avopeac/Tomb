#include "mesh_renderer.h"

#include <algorithm>

#include "renderer.h"

using namespace graphics;

MeshRenderer::MeshRenderer(GraphicsBase & graphics_base) :
	graphics_base_(graphics_base)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Push(size_t mesh_hash, size_t texture_hash, const glm::mat4 &model, const std::function<void(Program *, Program *)> &update)
{
	auto &mesh_cache = ResourceManager::Get().GetMeshCache();
	auto &texture_cache = ResourceManager::Get().GetTextureCache();

	MeshRenderInstance instance;
	instance.mesh = &mesh_cache.GetFromHash(mesh_hash);
	instance.texture = &texture_cache.GetFromHash(texture_hash);
	instance.model = model;
	instance.update = update;
	meshes_.push_back(instance);
}

void MeshRenderer::Draw(float delta_time)
{

	std::sort(meshes_.begin(), meshes_.end());
	
	RenderShadows();

	RenderGeometry();
	
	meshes_.clear();
}

void MeshRenderer::RenderShadows()
{
	auto &framebuffer_cache = ResourceManager::Get().GetFrameBufferCache();
	auto &program_cache = ResourceManager::Get().GetProgramCache();
	Program & shadow_vertex = program_cache.GetFromFile("shadow.vert", GL_VERTEX_SHADER, "assets/shaders/shadow.vert");
	Program & shadow_fragment = program_cache.GetFromFile("shadow.frag", GL_FRAGMENT_SHADER, "assets/shaders/shadow.frag");

	pipeline_.Bind();

	pipeline_.SetStages(shadow_vertex);
	pipeline_.SetStages(shadow_fragment);

	auto &shadow_map = framebuffer_cache.GetFromName(Renderer::shadow_map_name);
	shadow_map.BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0, 0, 0, 1);

	auto * shadow_camera = graphics_base_.GetShadowCamera();
	for (int i = 0; i < meshes_.size(); ++i)
	{
		auto &instance = meshes_[i];
		glm::mat4 mvp = shadow_camera->GetViewProj() * instance.model;
		shadow_vertex.SetUniform("u_mvp", (void*)glm::value_ptr(mvp));

		if (instance.update)
		{
			instance.update(&shadow_vertex, &shadow_fragment);
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

	shadow_map.UnbindDraw();
}

void MeshRenderer::RenderGeometry()
{

	auto &framebuffer_cache = ResourceManager::Get().GetFrameBufferCache();
	auto &program_cache = ResourceManager::Get().GetProgramCache();
	Program & main_vertex = program_cache.GetFromFile("deferred.vert", GL_VERTEX_SHADER, "assets/shaders/deferred.vert");
	Program & main_fragment = program_cache.GetFromFile("deferred.frag", GL_FRAGMENT_SHADER, "assets/shaders/deferred.frag");

	pipeline_.SetStages(main_vertex);
	pipeline_.SetStages(main_fragment);

	auto &shadow_map = framebuffer_cache.GetFromName(Renderer::shadow_map_name);
	auto &gbuffer = framebuffer_cache.GetFromName(Renderer::gbuffer_name);
	gbuffer.BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 shadow_bias = glm::translate(glm::mat4(1), glm::vec3(0.5f));
	shadow_bias *= glm::scale(glm::mat4(1), glm::vec3(0.5f));

	auto * main_camera = graphics_base_.GetMainCamera();
	auto * shadow_camera = graphics_base_.GetShadowCamera();
	for (int i = 0; i < meshes_.size(); ++i)
	{
		auto &instance = meshes_[i];
		int texture_id = 0;
		glm::mat4 mvp = main_camera->GetViewProj() * instance.model;
		glm::mat4 shadow_mvp = shadow_bias * shadow_camera->GetViewProj() * instance.model;
		glm::mat4 mv = main_camera->GetView() * instance.model;
		glm::mat4 normal_matrix = glm::inverse(glm::transpose(main_camera->GetView() * instance.model));

		auto &sampler_cache = ResourceManager::Get().GetSamplerCache();
		auto &sampler = sampler_cache.GetFromParameters(graphics::MagnificationFiltering::Linear,
			graphics::MinificationFiltering::LinearMipmapLinear,
			graphics::Wrapping::ClampToEdge, graphics::Wrapping::ClampToEdge);

		main_vertex.SetUniform("u_proj", (void*)glm::value_ptr(main_camera->GetProj()));
		main_vertex.SetUniform("u_view", (void*)glm::value_ptr(main_camera->GetView()));
		main_vertex.SetUniform("u_vp", (void*)glm::value_ptr(main_camera->GetViewProj()));
		main_vertex.SetUniform("u_mv", (void *)glm::value_ptr(mv));
		main_vertex.SetUniform("u_mvp", (void*)glm::value_ptr(mvp));
		main_vertex.SetUniform("u_model", (void*)glm::value_ptr(instance.model));
		main_vertex.SetUniform("u_shadow_mvp", (void*)glm::value_ptr(shadow_mvp));
		main_vertex.SetUniform("u_normal", (void*)glm::value_ptr(normal_matrix));

		int albedo_tex = 0;
		int shadow_tex = 1;
		main_fragment.SetUniform("u_texture", (void*)&albedo_tex);
		main_fragment.SetUniform("u_shadow", (void *)&shadow_tex);

		sampler.Bind(albedo_tex);
		sampler.Bind(shadow_tex);

		instance.texture->Bind(albedo_tex);
		shadow_map.BindDepthStencilAttachment(shadow_tex);

		if (instance.update)
		{
			instance.update(&main_vertex, &main_fragment);
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

	gbuffer.UnbindDraw();

	pipeline_.Unbind();
}
