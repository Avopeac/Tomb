#pragma once

#include <memory>

#include "graphics.h"
#include "frame_buffer.h"
#include "shader.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"
#include "sprite_renderer.h"
//#include "font_renderer.h"
#include "font_renderer_individual.h"
#include "post_processing.h"
#include "mesh.h"
#include "mesh_renderer.h"

#define SPRITE_INSTANCES_PER_BATCH 4000

namespace graphics
{
	class Renderer
	{

		GraphicsBase *graphics_base_;

		std::unique_ptr<FrameBufferCache> frame_buffer_cache_;
		std::unique_ptr<TextureCache> texture_cache_;
		std::unique_ptr<SamplerCache> sampler_cache_;
		std::unique_ptr<ProgramCache> program_cache_;
		std::unique_ptr<BlendCache> blend_cache_;
		std::unique_ptr<MeshCache> mesh_cache_;
		
		std::unique_ptr<SpriteRenderer> sprite_renderer_;
		std::unique_ptr<MeshRenderer> mesh_renderer_;
		std::unique_ptr<PostProcessing> post_processing_;
		std::unique_ptr<FontRendererIndividual> font_renderer_;

		FrameBuffer * gbuffer_;
		FrameBuffer * gbuffer_comp_;

	public:

		static const std::string gbuffer_name;
		static const std::string gbuffer_composition_name;

		Renderer(GraphicsBase *graphics_base);

		~Renderer();

		inline SpriteRenderer &GetSpriteRenderer() { return *sprite_renderer_; }

		inline MeshRenderer &GetMeshRenderer() { return *mesh_renderer_; }

		inline FontRendererIndividual &GetFontRenderer() { return *font_renderer_; }

		inline PostProcessing &GetPostProcessing() { return *post_processing_; }

		inline FrameBufferCache &GetFrameBufferCache() { return *frame_buffer_cache_; }

		inline TextureCache &GetTextureCache() { return *texture_cache_; }

		inline SamplerCache &GetSamplerCache() { return *sampler_cache_; }

		inline ProgramCache &GetProgramCache() { return *program_cache_; }

		inline BlendCache &GetBlendCache() { return *blend_cache_; }

		inline MeshCache &GetMeshCache() { return *mesh_cache_; }

		void Invoke(float frame_time);

	private:

		FrameBuffer * MakeGbuffer();
		
		FrameBuffer * MakeGbufferComposition();
	};
}
