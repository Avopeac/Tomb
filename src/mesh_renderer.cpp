#include "mesh_renderer.h"

#include <algorithm>

#include "renderer.h"

#include "fullscreen_quad.h"

#include "data_pipe_hub.h"

using namespace graphics;

const std::string MeshRenderer::shadow_map_name = "shadow_map";
const std::string MeshRenderer::gbuffer_name = "gbuffer";
const std::string MeshRenderer::gbuffer_composition_name = "gbuffer_comp";
const std::string MeshRenderer::vsm_shadow_map_vertical_name = "vsm_vert_shadow_map";
const std::string MeshRenderer::vsm_shadow_map_horizontal_name = "vsm_horiz_shadow_map";

MeshRenderer::MeshRenderer(GraphicsBase & graphics_base) :
	graphics_base_(graphics_base)
{

	main_camera_ = graphics_base_.GetMainCamera();
	shadow_camera_ = graphics_base_.GetShadowCamera();

	shadow_map_ = MakeShadowMap();
	vsm_shadow_map_horizontal_ = MakeVsmShadowMap();
	vsm_shadow_map_vertical_ = MakeVsmShadowMap();
	gbuffer_ = MakeGbuffer();
	gbuffer_composition_ = MakeGbufferComposition();

	auto &program_cache = ResourceManager::Get().GetProgramCache();
	geometry_vertex_ = program_cache.GetFromFile("deferred.vert", GL_VERTEX_SHADER, "assets/shaders/deferred.vert");
	geometry_fragment_ = program_cache.GetFromFile("deferred.frag", GL_FRAGMENT_SHADER, "assets/shaders/deferred.frag");
	shadow_vertex_ = program_cache.GetFromFile("shadow.vert", GL_VERTEX_SHADER, "assets/shaders/shadow.vert");
	shadow_fragment_ = program_cache.GetFromFile("shadow.frag", GL_FRAGMENT_SHADER, "assets/shaders/shadow.frag");
	gbuffer_composition_vertex_ = program_cache.GetFromFile("gbuffer_comp.vert", GL_VERTEX_SHADER, "assets/shaders/gbuffer_comp.vert");
	gbuffer_composition_fragment_ = program_cache.GetFromFile("gbuffer_comp.frag", GL_FRAGMENT_SHADER, "assets/shaders/gbuffer_comp.frag");
	vsm_shadow_map_copy_vertex_ = program_cache.GetFromFile("vsm_shadow_copy.vert", GL_VERTEX_SHADER, "assets/shaders/vsm_shadow_copy.vert");
	vsm_shadow_map_copy_fragment_ = program_cache.GetFromFile("vsm_shadow_copy.frag", GL_FRAGMENT_SHADER, "assets/shaders/vsm_shadow_copy.frag");
	vsm_shadow_map_blur_vertex_ = program_cache.GetFromFile("vsm_shadow_blur.vert", GL_VERTEX_SHADER, "assets/shaders/vsm_shadow_blur.vert");
	vsm_shadow_map_blur_fragment_ = program_cache.GetFromFile("vsm_shadow_blur.frag", GL_FRAGMENT_SHADER, "assets/shaders/vsm_shadow_blur.frag");
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Draw(float delta_time)
{
	PreProcessGeometry();

	pipeline_.Bind();

	RenderShadows();

	RenderVsmShadows();

	RenderGeometry();

	RenderComposition();

	pipeline_.Unbind();

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

	glBindVertexArray(0);

	shadow_map_->UnbindDraw();
}

void MeshRenderer::RenderVsmShadows()
{
	int32_t shadow_index = 0;
	
	pipeline_.SetStages(*vsm_shadow_map_copy_vertex_);
	pipeline_.SetStages(*vsm_shadow_map_copy_fragment_);

	vsm_shadow_map_copy_fragment_->SetUniform("u_shadow", (void*)&shadow_index);

	shadow_map_->UnbindDraw();
	vsm_shadow_map_vertical_->BindDraw(GL_COLOR_BUFFER_BIT, 0, 0, 0, 1);

	shadow_map_->BindDepthStencilAttachment(shadow_index);

	FullscreenQuad::Get().Begin();
	FullscreenQuad::Get().DrawElements();
	FullscreenQuad::Get().End();

	shadow_map_->UnbindDepthStencilAttachment();

	vsm_shadow_map_vertical_->UnbindDraw();

	pipeline_.SetStages(*vsm_shadow_map_blur_vertex_);
	pipeline_.SetStages(*vsm_shadow_map_blur_fragment_);

	FrameBuffer * ping = vsm_shadow_map_horizontal_;
	FrameBuffer * pong = vsm_shadow_map_vertical_;
	
	glm::vec2 texel_size = 1.0f / 
		glm::vec2(graphics_base_.GetBackbufferWidth(), graphics_base_.GetBackbufferHeight());

	for (int i = 0; i < 2; ++i)
	{
		
		int dir = i % 2;
		vsm_shadow_map_blur_fragment_->SetUniform("u_vsm_shadow", (void*)&shadow_index);
		vsm_shadow_map_blur_fragment_->SetUniform("u_direction", (void *)&dir);
		vsm_shadow_map_blur_fragment_->SetUniform("u_texel_size", (void *)&texel_size);

		ping->BindDraw(0, 0, 0, 0, 0);
		
		pong->BindColorAttachment(shadow_index, shadow_index);

		FullscreenQuad::Get().Begin();
		FullscreenQuad::Get().DrawElements();
		FullscreenQuad::Get().End();

		pong->UnbindColorAttachment(shadow_index);

		ping->UnbindDraw();

		FrameBuffer * temp = ping;
		ping = pong;
		pong = ping;

		vsm_shadow_map_ = pong;
	}

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
		vsm_shadow_map_->BindColorAttachment(0, shadow_tex);

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

	glBindVertexArray(0);

	gbuffer_->UnbindDraw();
}

void MeshRenderer::RenderComposition()
{
	static bool once = false;
	static GLuint skybox;

	int32_t albedo_index = 0;
	int32_t position_index = 1;
	int32_t normal_index = 2;
	int32_t shadow_index = 3;
	int32_t depth_index = 4;
	int32_t skybox_index = 5;

	pipeline_.SetStages(*gbuffer_composition_vertex_);
	pipeline_.SetStages(*gbuffer_composition_fragment_);

	if (!once)
	{
		
		gbuffer_composition_fragment_->SetUniform("u_albedo", (void *)&albedo_index);
		gbuffer_composition_fragment_->SetUniform("u_position", (void *)&position_index);
		gbuffer_composition_fragment_->SetUniform("u_normal", (void *)&normal_index);
		gbuffer_composition_fragment_->SetUniform("u_shadow", (void *)&shadow_index);
		gbuffer_composition_fragment_->SetUniform("u_depth", (void*)&depth_index);
		gbuffer_composition_fragment_->SetUniform("u_skybox", (void*)&skybox_index);

		auto &texture_cache = ResourceManager::Get().GetTextureCache();
		auto * texture = texture_cache.GetFromFile("assets/textures/violentdays_large_2.png", 0);

		uint8_t * pos_x_data = texture->GetSubresourceData(2048, 1024, 1024, 1024);
		uint8_t * neg_x_data = texture->GetSubresourceData(0, 1024, 1024, 1024);
		uint8_t * pos_y_data = texture->GetSubresourceData(1024, 0, 1024, 1024);
		uint8_t * neg_y_data = texture->GetSubresourceData(1024, 2048, 1024, 1024);
		uint8_t * pos_z_data = texture->GetSubresourceData(1024, 1024, 1024, 1024);
		uint8_t * neg_z_data = texture->GetSubresourceData(3072, 1024, 1024, 1024);

		GLfloat border[4]{ 1.0f, 1.0f, 1.0f, 1.0f };

		glGenTextures(1, &skybox);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &border[0]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pos_x_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)neg_x_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pos_y_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)neg_y_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)pos_z_data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)neg_z_data);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		delete pos_x_data;
		delete neg_x_data;
		delete pos_y_data;
		delete neg_y_data;
		delete pos_z_data;
		delete neg_z_data;

		once = true;
	}


	gbuffer_->UnbindDraw();
	gbuffer_composition_->BindDraw(GL_COLOR_BUFFER_BIT, 0, 0, 0, 1);

	gbuffer_->BindColorAttachment(albedo_index, albedo_index);
	gbuffer_->BindColorAttachment(position_index, position_index);
	gbuffer_->BindColorAttachment(normal_index, normal_index);
	gbuffer_->BindColorAttachment(shadow_index, shadow_index);
	gbuffer_->BindDepthStencilAttachment(depth_index);

	glActiveTexture(GL_TEXTURE0 + skybox_index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);

	glm::mat4 view_no_pos = main_camera_->GetView();
	view_no_pos[3] = glm::vec4(0, 0, 0, 1);

	gbuffer_composition_vertex_->SetUniform("u_view",
		(void *)glm::value_ptr(view_no_pos));
	gbuffer_composition_fragment_->SetUniform("u_view",
		(void *)glm::value_ptr(view_no_pos));
	gbuffer_composition_fragment_->SetUniform("u_inv_view",
		(void *)glm::value_ptr(glm::inverse(view_no_pos)));
	gbuffer_composition_fragment_->SetUniform("u_inv_proj",
		(void *)glm::value_ptr(main_camera_->GetInvProj()));

	FullscreenQuad::Get().Begin();
	FullscreenQuad::Get().DrawElements();
	FullscreenQuad::Get().End();
	
	gbuffer_->UnbindColorAttachment(albedo_index);
	gbuffer_->UnbindColorAttachment(position_index);
	gbuffer_->UnbindColorAttachment(normal_index);
	gbuffer_->UnbindColorAttachment(shadow_index);
	gbuffer_->UnbindDepthStencilAttachment();

	glActiveTexture(GL_TEXTURE0 + skybox_index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	gbuffer_composition_->UnbindDraw();
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

FrameBuffer * MeshRenderer::MakeGbuffer()
{
	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor albedo;
	albedo.format = GL_RGB;
	albedo.internal_format = GL_RGB16F;
	albedo.type = GL_FLOAT;

	FrameBufferAttachmentDescriptor position;
	position.format = GL_RGB;
	position.internal_format = GL_RGB16F;
	position.type = GL_FLOAT;

	FrameBufferAttachmentDescriptor normals;
	normals.format = GL_RGB;
	normals.internal_format = GL_RGB16F;
	normals.type = GL_FLOAT;

	FrameBufferAttachmentDescriptor shadow;
	shadow.format = GL_RED;
	shadow.internal_format = GL_R8;
	shadow.type = GL_UNSIGNED_INT;

	FrameBufferAttachmentDescriptor depth;
	depth.format = GL_DEPTH_COMPONENT;
	depth.internal_format = GL_DEPTH_COMPONENT24;
	depth.type = GL_FLOAT;

	descriptors.push_back(albedo);
	descriptors.push_back(position);
	descriptors.push_back(normals);
	descriptors.push_back(shadow);

	auto &frame_buffer_cache = ResourceManager::Get().GetFrameBufferCache();
	return frame_buffer_cache.GetFromParameters(gbuffer_name,
		graphics_base_.GetBackbufferWidth(), graphics_base_.GetBackbufferHeight(),
		0, descriptors, &depth);

}

FrameBuffer * MeshRenderer::MakeGbufferComposition()
{
	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor composition;
	composition.format = GL_RGB;
	composition.internal_format = GL_RGB16F;
	composition.type = GL_FLOAT;

	descriptors.push_back(composition);

	auto &frame_buffer_cache = ResourceManager::Get().GetFrameBufferCache();
	return frame_buffer_cache.GetFromParameters(gbuffer_composition_name,
		graphics_base_.GetBackbufferWidth(), graphics_base_.GetBackbufferHeight(),
		0, descriptors, nullptr);
}

FrameBuffer * MeshRenderer::MakeShadowMap()
{
	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor depth;
	depth.format = GL_DEPTH_COMPONENT;
	depth.internal_format = GL_DEPTH_COMPONENT24;
	depth.type = GL_FLOAT;

	auto &frame_buffer_cache = ResourceManager::Get().GetFrameBufferCache();

	return frame_buffer_cache.GetFromParameters(shadow_map_name,
		graphics_base_.GetBackbufferWidth(), graphics_base_.GetBackbufferHeight(),
		0, descriptors, &depth);
}

FrameBuffer * MeshRenderer::MakeVsmShadowMap()
{
	std::vector<FrameBufferAttachmentDescriptor> descriptors;

	FrameBufferAttachmentDescriptor vsm_shadow;
	vsm_shadow.format = GL_RG;
	vsm_shadow.internal_format = GL_RG32F;
	vsm_shadow.type = GL_FLOAT;

	descriptors.push_back(vsm_shadow);

	auto &frame_buffer_cache = ResourceManager::Get().GetFrameBufferCache();
	return frame_buffer_cache.GetFromParameters(gbuffer_composition_name,
		graphics_base_.GetBackbufferWidth(), graphics_base_.GetBackbufferHeight(),
		0, descriptors, nullptr);
}
