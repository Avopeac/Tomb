#pragma once

#include <unordered_map>
#include <string>
#include "graphics.h"
#include "resource_manager.h"

namespace graphics
{
	struct MeshRenderInstance
	{
		Mesh * mesh;
		Texture * texture;
		GLuint world_transform_buffer;
		std::vector<glm::mat4> world_transforms;

		bool operator<(const MeshRenderInstance &other) const
		{
			if (!other.mesh || !other.texture)
				return false;

			return mesh->vao < other.mesh->vao;
		}
	};

	class MeshRenderer
	{
		ProgramPipeline pipeline_;

		Program * geometry_vertex_;
		Program * geometry_fragment_;

		Program * shadow_vertex_;
		Program * shadow_fragment_;

		Program * gbuffer_composition_vertex_;
		Program * gbuffer_composition_fragment_;

		Program * vsm_shadow_map_copy_vertex_;
		Program * vsm_shadow_map_copy_fragment_;

		Program * vsm_shadow_map_blur_vertex_;
		Program * vsm_shadow_map_blur_fragment_;

		FrameBuffer * shadow_map_;
		FrameBuffer * vsm_shadow_map_;
		FrameBuffer * gbuffer_;
		FrameBuffer * gbuffer_composition_;
		FrameBuffer * vsm_shadow_map_vertical_;
		FrameBuffer * vsm_shadow_map_horizontal_;

		AbstractCamera * shadow_camera_;
		AbstractCamera * main_camera_;

		GraphicsBase & graphics_base_;

		std::unordered_map<size_t, MeshRenderInstance> meshes_;

	public:

		static const std::string shadow_map_name;
		static const std::string gbuffer_name;
		static const std::string gbuffer_composition_name;
		static const std::string vsm_shadow_map_vertical_name;
		static const std::string vsm_shadow_map_horizontal_name;

		MeshRenderer(GraphicsBase & graphics_base);

		~MeshRenderer();

		void Draw(float delta_time);

	private: 

		void RenderShadows();

		void RenderVsmShadows();

		void RenderGeometry();

		void RenderComposition();

		void PreProcessGeometry();

		FrameBuffer * MakeGbuffer();

		FrameBuffer * MakeGbufferComposition();

		FrameBuffer * MakeShadowMap();

		FrameBuffer * MakeVsmShadowMap(const std::string &name);
	};
}