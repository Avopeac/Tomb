#pragma once

#include "SDL.h"

#include "GL/glew.h"

#include "disposable.h"
#include "texture.h"

namespace graphics
{
	class FrameBuffer : public base::Disposable
	{

		GLuint id_;

		Uint32 width_;

		Uint32 height_;

		Uint32 old_viewport_width_;

		Uint32 old_viewport_height_;

		std::vector<GLuint> textures_;

		GLuint depth_texture_;

	public:

		FrameBuffer(Uint32 width, Uint32 height, bool depth, bool dynamic_range_enabled);

		~FrameBuffer();

		void BindColorAttachmentTexture(size_t index);

		void UnbindColorAttachmentTexture(size_t index);

		void BindDepthAttachmentTexture();

		void UnbindDepthAttachmentTexture();

		void BindDraw();

		void UnbindDraw();

		// Inherited via Disposable
		virtual void Free() override;

		inline Uint32 GetWidth() const { return width_; }

		inline Uint32 GetHeight() const { return height_; }
		
	};
}
