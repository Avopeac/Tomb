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
	shadow_map_ = framebuffer_cache.GetFromName(Renderer::shadow_map_name);
	gbuffer_ = framebuffer_cache.GetFromName(Renderer::gbuffer_name);

	auto &program_cache = ResourceManager::Get().GetProgramCache();
	geometry_vertex_ = program_cache.GetFromFile("deferred.vert", GL_VERTEX_SHADER, "assets/shaders/deferred.vert");
	geometry_fragment_ = program_cache.GetFromFile("deferred.frag", GL_FRAGMENT_SHADER, "assets/shaders/deferred.frag");
	shadow_vertex_ = program_cache.GetFromFile("shadow.vert", GL_VERTEX_SHADER, "assets/shaders/shadow.vert");
	shadow_fragment_ = program_cache.GetFromFile("shadow.frag", GL_FRAGMENT_SHADER, "assets/shaders/shadow.frag");

}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Draw(float delta_time)
{
	PreProcessGeometry();

	pipeline_.Bind();

	RenderShadows();

	RenderGeometry();

	pipeline_.Unbind();

	glBindVertexArray(0);
	
	for (auto &it : meshes_)
	{
		glDeleteBuffers(1, &it.second.world_transform_buffer);
	}

	meshes_.clear();
}

void MeshRenderer::RenderShadows()
{
	pipeline_.SetStages(*shadow_vertex_);
	pipeline_.SetStages(*shadow_fragment_);
	shadow_map_->BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0, 0, 0, 1);

	if (meshes_.size() > 0)
	{
		shadow_vertex_->SetUniform("u_vp", (void*)glm::value_ptr(shadow_camera_->GetViewProj()));
		for (auto &it : meshes_) 
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, it.second.world_transform_buffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, it.second.world_transform_buffer);
			glBindVertexArray(it.second.mesh->vao);
			glDrawElementsInstanced(GL_TRIANGLES, it.second.mesh->num_indices[0], GL_UNSIGNED_INT, (void*)0, it.second.world_transforms.size());
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}

	shadow_map_->UnbindDraw();
}

void MeshRenderer::RenderGeometry()
{
	auto &sampler_cache = ResourceManager::Get().GetSamplerCache();
	auto * sampler = sampler_cache.GetFromParameters(MagnificationFiltering::Linear,
		MinificationFiltering::LinearMipmapLinear,
		Wrapping::ClampToEdge, Wrapping::ClampToEdge);

	glm::mat4 shadow_bias = glm::translate(glm::mat4(1), glm::vec3(0.5f));
	shadow_bias *= glm::scale(glm::mat4(1), glm::vec3(0.5f));

	glm::vec3 color = glm::vec3(1.0f);

	pipeline_.SetStages(*geometry_vertex_);
	pipeline_.SetStages(*geometry_fragment_);
	gbuffer_->BindDraw(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, 0.0f, 0.0f, 0.0f, 1.0f); 

	if (meshes_.size() > 0)
	{
		MainCamera * camera = (MainCamera *)main_camera_;
		glm::mat4 shadow_vp = shadow_bias * shadow_camera_->GetViewProj();
		geometry_vertex_->SetUniform("u_proj", (void*)glm::value_ptr(camera->GetProj()));
		geometry_vertex_->SetUniform("u_view", (void*)glm::value_ptr(camera->GetView()));
		geometry_vertex_->SetUniform("u_vp", (void*)glm::value_ptr(camera->GetViewProj()));
		geometry_vertex_->SetUniform("u_shadow_vp", (void*)glm::value_ptr(shadow_vp));
		geometry_vertex_->SetUniform("u_color", (void*)glm::value_ptr(color));

		int albedo_tex = 0;
		int shadow_tex = 1;
		geometry_fragment_->SetUniform("u_texture", (void*)&albedo_tex);
		geometry_fragment_->SetUniform("u_shadow", (void *)&shadow_tex);

		sampler->Bind(albedo_tex);
		sampler->Bind(shadow_tex);
		shadow_map_->BindDepthStencilAttachment(shadow_tex);

		for (auto &it : meshes_)
		{
			it.second.texture->Bind(albedo_tex); 
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, it.second.world_transform_buffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, it.second.world_transform_buffer);
			glBindVertexArray(it.second.mesh->vao);
			glDrawElementsInstanced(GL_TRIANGLES, it.second.mesh->num_indices[0], GL_UNSIGNED_INT, (void*)0,
				it.second.world_transforms.size());
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}

	gbuffer_->UnbindDraw();

	pipeline_.Unbind();
}
 
void MeshRenderer::PreProcessGeometry()
{
	auto &mesh_data_hub = DataPipeHub::Get().GetMeshDataPipe();
	auto &mesh_cache = ResourceManager::Get().GetMeshCache();
	auto &texture_cache = ResourceManager::Get().GetTextureCache();

	for (auto &it : mesh_data_hub.GetData())
	{
		MeshRenderInstance * instance = nullptr;
		if (meshes_.find(it.mesh_hash) == meshes_.end())
		{
			meshes_[it.mesh_hash] = MeshRenderInstance();
			instance = &meshes_[it.mesh_hash];
			instance->mesh = mesh_cache.GetFromHash(it.mesh_hash);
			instance->texture = texture_cache.GetFromHash(it.texture_hash);
		}
		else
		{
			instance = &meshes_[it.mesh_hash];
		}

		instance->world_transforms.push_back(it.world_transform);
	}

	mesh_data_hub.Flush();

	for (auto &it : meshes_)
	{
		glGenBuffers(1, &it.second.world_transform_buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, it.second.world_transform_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, it.second.world_transforms.size() * sizeof(glm::mat4),
			it.second.world_transforms.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}
