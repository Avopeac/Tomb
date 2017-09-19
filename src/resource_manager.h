#pragma once

#include <memory>

#include "blend_mode.h"
#include "frame_buffer.h"
#include "mesh.h"
#include "sampler.h"
#include "shader.h"
#include "texture.h"

namespace graphics
{

	class ResourceManager
	{

		ResourceManager * this_ptr = nullptr;

		std::unique_ptr<BlendCache> blend_cache_;
		std::unique_ptr<FrameBufferCache> frame_buffer_cache_;
		std::unique_ptr<MeshCache> mesh_cache_;
		std::unique_ptr<SamplerCache> sampler_cache_;
		std::unique_ptr<ProgramCache> program_cache_;
		std::unique_ptr<TextureCache> texture_cache_;

	public:

		static ResourceManager &Get()
		{
			static ResourceManager instance;

			// NOTE: Not thread safe
			if (instance.this_ptr == nullptr)
			{
				instance.Initialize();
				instance.this_ptr = &instance;
			}

			return instance;
		}

		~ResourceManager();

		inline BlendCache &GetBlendCache() { return *blend_cache_.get(); }

		inline FrameBufferCache &GetFrameBufferCache() { return *frame_buffer_cache_.get(); }

		inline MeshCache &GetMeshCache() { return *mesh_cache_.get(); }

		inline SamplerCache &GetSamplerCache() { return *sampler_cache_.get(); }

		inline ProgramCache &GetProgramCache() { return *program_cache_.get(); }

		inline TextureCache &GetTextureCache() { return *texture_cache_.get(); }

	private:

		ResourceManager();

		void Initialize();

	};
}
