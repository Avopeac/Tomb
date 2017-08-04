#pragma once

#include <memory>

#include "SDL.h"

#include "GL/glew.h"

#include "disposable.h"
#include "texture.h"

namespace graphics
{
	struct FrameBufferAttachmentDescriptor
	{
		GLenum internal_format, format, type;
	};

	class FrameBufferAttachment : public base::Disposable
	{
		int bind_point_; 

		GLuint id_;

		Uint32 width_, height_, num_samples_;

		FrameBufferAttachmentDescriptor descriptor_;

	public:

		FrameBufferAttachment(uint32_t width, uint32_t height, uint32_t num_samples, 
			const FrameBufferAttachmentDescriptor &descriptor);

		~FrameBufferAttachment();

		// Inherited via Disposable
		void Free() override;

		inline GLuint GetId() const { return id_; }

		void Bind(Uint32 bind_point);

		void Unbind();

		inline Uint32 GetWidth() const { return width_; }

		inline Uint32 GetHeight() const { return height_; }

		inline Uint32 GetNumSamples() const { return num_samples_; }
		
		inline const FrameBufferAttachmentDescriptor &GetDescriptor() const { return descriptor_; }

	};

	class FrameBuffer : public base::Disposable
	{

		GLuint id_;

		Uint32 width_, height_, num_samples_;

		Uint32 old_viewport_width_, old_viewport_height_;

		std::vector<std::unique_ptr<FrameBufferAttachment>> attachments_;

		std::unique_ptr<FrameBufferAttachment> depth_stencil_attachment_;

	public:

		FrameBuffer(Uint32 width, Uint32 height, Uint32 num_samples,
			const std::vector<FrameBufferAttachmentDescriptor> &descriptors,
			const FrameBufferAttachmentDescriptor * const depth_stencil_descriptor);
		
		~FrameBuffer();

		void Blit(Sint32 sx0, Sint32 sx1, Sint32 sy0, Sint32 sy1,
			Sint32 dx0, Sint32 dx1, Sint32  dy0, Sint32 dy1, FrameBuffer * fbo);

		void BindDraw(GLbitfield clear_flags, float r, float g, float b, float a);

		void UnbindDraw();

		// Inherited via Disposable
		virtual void Free() override;

		inline size_t GetColorAttachmentCount() const { return attachments_.size(); }

		inline bool HasDepthStencil() const { return depth_stencil_attachment_ != nullptr; }

		inline Uint32 GetWidth() const { return width_; }

		inline Uint32 GetHeight() const { return height_; }
		
	};

	class FrameBufferCache
	{
		std::unordered_map<size_t, FrameBuffer> buffers_;

	public:
		
		FrameBufferCache();

		~FrameBufferCache();

		FrameBuffer &GetFromParameters(const std::string &name, size_t &out_hash,
			Uint32 width, Uint32 height, Uint32 num_samples,
			const std::vector<FrameBufferAttachmentDescriptor> &descriptors,
			const FrameBufferAttachmentDescriptor * const depth_stencil_descriptor);

		FrameBuffer &GetFromName(const std::string &name);

		FrameBuffer &GetFromHash(size_t hash);

	};
}
