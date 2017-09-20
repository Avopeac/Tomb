#pragma once

#include <memory>

#include "graphics.h"
#include "post_processing.h"
#include "mesh_renderer.h"

namespace graphics
{
	class Renderer
	{

		GraphicsBase *graphics_base_;

		std::unique_ptr<MeshRenderer> mesh_renderer_;
		std::unique_ptr<PostProcessing> post_processing_;

		FrameBuffer * shadow_map_;
		FrameBuffer * gbuffer_;
		FrameBuffer * gbuffer_comp_;

	public:

		static const std::string shadow_map_name;
		static const std::string gbuffer_name;
		static const std::string gbuffer_composition_name;

		Renderer(GraphicsBase *graphics_base);

		~Renderer();

		inline MeshRenderer &GetMeshRenderer() { return *mesh_renderer_; }

		inline PostProcessing &GetPostProcessing() { return *post_processing_; }

		void Invoke(float frame_time);

	private:

		FrameBuffer * MakeGbuffer();
		
		FrameBuffer * MakeGbufferComposition();

		FrameBuffer * MakeShadowMap();
	};
}
