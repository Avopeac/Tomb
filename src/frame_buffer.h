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

		std::vector<GLint> texture_bind_points_;

		GLuint depth_texture_;

		GLint depth_texture_bind_point_;

		int num_samples_;

	public:

		FrameBuffer(Uint32 width, Uint32 height, bool depth, bool dynamic_range_enabled, int num_samples = 0);

		~FrameBuffer();

		void BindColorAttachmentTexture(size_t index, GLuint unit);

		void UnbindColorAttachmentTexture(size_t index);

		void BindDepthAttachmentTexture(GLuint unit);

		void UnbindDepthAttachmentTexture();

		void Blit(Sint32 sx0, Sint32 sx1, Sint32 sy0, Sint32 sy1,
			Sint32 dx0, Sint32 dx1, Sint32  dy0, Sint32 dy1);

		void BindDraw();

		void UnbindDraw();

		// Inherited via Disposable
		virtual void Free() override;

		inline size_t GetColorAttachmentCount() const { return textures_.size(); }

		inline Uint32 GetWidth() const { return width_; }

		inline Uint32 GetHeight() const { return height_; }
		
	};
}
