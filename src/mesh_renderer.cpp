#include "mesh_renderer.h"

#include <algorithm>

#include "renderer.h"

#include "data_pipe_hub.h"

using namespace graphics;

MeshRenderer::MeshRenderer(GraphicsBase & graphics_base) :
	graphics_base_(graphics_base)
{

	main_camera_ = graphics_base_.GetMainCamera();
	shadow_camera_ = graphics_base.GetShadowCamera();

	auto &framebuffer_cache = ResourceManager::Get().GetFrameBufferCache();
	shadow_map_ = &framebuffer_cache.GetFromName(Renderer::shadow_map_name);
	gbuffer_ = &framebuffer_cache.GetFromName(Renderer::gbuffer_name);

	auto &program_cache = ResourceManager::Get().GetProgramCache();
	geometry_vertex_ = &program_cache.GetFromFile("deferred.vert", GL_VERTEX_SHADER, "assets/shaders/deferred.vert");
	geometry_fragment_ = &program_cache.GetFromFile("deferred.frag", GL_FRAGMENT_SHADER, "assets/shaders/deferred.frag");
	shadow_vertex_ = &program_cache.GetFromFile("shadow.vert", GL_VERTEX_SHADER, "assets/shaders/shadow.vert");
	shadow_fragment_ = &program_cache.GetFromFile("shadow.frag", GL_FRAGMENT_SHADER, "assets/shaders/shadow.frag");

}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Draw(float delta_time)
{
	auto &mesh_data_hub = DataPipeHub::Get().GetMeshDataPipe();
	auto &mesh_cache = ResourceManager::Get().GetMeshCache();
	auto &texture_cache = ResourceManager::Get().GetTextureCache();

	for (auto &it : mesh_data_hub.GetData())
	{
		MeshRenderInstance instance;
		instance.mesh = &mesh_cache.GetFromHash(it.mesh_hash);
		instance.texture = &texture_cache.GetFromHash(it.texture_hash);
		instance.model = it.world_transform;
		meshes_.push_back(instance);
	}

	mesh_data_hub.Flush();

	std::sort(meshes_.begin(), meshes_.end());

	pipeline_.Bind();

	RenderShadows();

	RenderGeometry();

	pipeline_.Unbind();

	glBindVertexArray(0);
	
	meshes_.clear();
}

void MeshRenderer::RenderShadows()
{
	
	pipeline_.SetStages(*shadow_vertex_);
	pipeline_.SetStages(*shadow_fragment_);
	shadow_map_->BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0, 0, 0, 1);

	for (int i = 0; i < meshes_.size(); ++i)
	{
		auto &instance = meshes_[i];

		// Use shadow view / proj matrix and do a depth only pass
		glm::mat4 mvp = shadow_camera_->GetViewProj() * instance.model;
		shadow_vertex_->SetUniform("u_mvp", (void*)glm::value_ptr(mvp));

		glBindVertexArray(instance.mesh->vao);
		size_t index_offset = 0;
		for (Uint32 j = 0; j < instance.mesh->num_meshes; ++j)
		{
			glDrawElements(GL_TRIANGLES, (GLsizei)instance.mesh->num_indices[j], GL_UNSIGNED_INT, (void*)index_offset);
			index_offset += instance.mesh->num_indices[j] * sizeof(Uint32);
		}
	}

	shadow_map_->UnbindDraw();
}

void MeshRenderer::RenderGeometry()
{
	auto &sampler_cache = ResourceManager::Get().GetSamplerCache();
	auto &sampler = sampler_cache.GetFromParameters(MagnificationFiltering::Linear,
		MinificationFiltering::LinearMipmapLinear,
		Wrapping::ClampToEdge, Wrapping::ClampToEdge);

	glm::mat4 shadow_bias = glm::translate(glm::mat4(1), glm::vec3(0.5f));
	shadow_bias *= glm::scale(glm::mat4(1), glm::vec3(0.5f));

	pipeline_.SetStages(*geometry_vertex_);
	pipeline_.SetStages(*geometry_fragment_);
	gbuffer_->BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < meshes_.size(); ++i)
	{
		auto &instance = meshes_[i];
		
		glm::mat4 mvp = main_camera_->GetViewProj() * instance.model;
		glm::mat4 shadow_mvp = shadow_bias * shadow_camera_->GetViewProj() * instance.model;
		glm::mat4 mv = main_camera_->GetView() * instance.model;
		glm::mat4 normal_matrix = glm::inverse(glm::transpose(main_camera_->GetView() * instance.model));

		geometry_vertex_->SetUniform("u_proj", (void*)glm::value_ptr(main_camera_->GetProj()));
		geometry_vertex_->SetUniform("u_view", (void*)glm::value_ptr(main_camera_->GetView()));
		geometry_vertex_->SetUniform("u_vp", (void*)glm::value_ptr(main_camera_->GetViewProj()));
		geometry_vertex_->SetUniform("u_mv", (void *)glm::value_ptr(mv));
		geometry_vertex_->SetUniform("u_mvp", (void*)glm::value_ptr(mvp));
		geometry_vertex_->SetUniform("u_model", (void*)glm::value_ptr(instance.model));
		geometry_vertex_->SetUniform("u_shadow_mvp", (void*)glm::value_ptr(shadow_mvp));
		geometry_vertex_->SetUniform("u_normal", (void*)glm::value_ptr(normal_matrix));

		int albedo_tex = 0;
		int shadow_tex = 1;
		geometry_fragment_->SetUniform("u_texture", (void*)&albedo_tex);
		geometry_fragment_->SetUniform("u_shadow", (void *)&shadow_tex);

		sampler.Bind(albedo_tex);
		sampler.Bind(shadow_tex);
		instance.texture->Bind(albedo_tex);
		shadow_map_->BindDepthStencilAttachment(shadow_tex);

		glBindVertexArray(instance.mesh->vao);
		size_t index_offset = 0;
		for (Uint32 j = 0; j < instance.mesh->num_meshes; ++j)
		{
			glDrawElements(GL_TRIANGLES, (GLsizei)instance.mesh->num_indices[j], GL_UNSIGNED_INT, (void*)index_offset);
			index_offset += instance.mesh->num_indices[j] * sizeof(Uint32);
		}
	}

	gbuffer_->UnbindDraw();

	pipeline_.Unbind();
}
