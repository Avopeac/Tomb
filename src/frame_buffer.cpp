#include "frame_buffer.h"

#include "texture.h"
#include "logger.h"

using namespace graphics;

FrameBuffer::FrameBuffer(Uint32 width, Uint32 height, bool depth, bool dynamic_range_enabled)
	: width_(width), height_(height), depth_texture_(0),
	old_viewport_width_(0), old_viewport_height_(0)
{
	GLenum internalFormat = dynamic_range_enabled ? GL_RGBA16F : GL_RGBA8;
	GLenum format = dynamic_range_enabled ? GL_FLOAT : GL_UNSIGNED_BYTE;

	GLuint color_attachment;
	glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment);
	glTextureStorage2D(color_attachment, 1, internalFormat, width, height);
	glTextureSubImage2D(color_attachment, 0, 0, 0, width, height,
		GL_RGBA, format, nullptr);
	glTextureParameteri(color_attachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(color_attachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(color_attachment, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(color_attachment, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	textures_.push_back(color_attachment);

	if (depth)
	{
		GLuint depth_attachment;
		glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment);
		glTextureStorage2D(depth_attachment, 0, GL_DEPTH_COMPONENT16, width, height);
		glTextureSubImage2D(depth_attachment, 0, 0, 0, width, height,
			GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		glTextureParameteri(depth_attachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(depth_attachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(depth_attachment, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(depth_attachment, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		depth_texture_ = depth_attachment;
	}

	glCreateFramebuffers(1, &id_);
	glNamedFramebufferTexture(id_, GL_COLOR_ATTACHMENT0, textures_.back(), 0);
	glNamedFramebufferTexture(id_, GL_DEPTH_ATTACHMENT, depth_texture_, 0);
	
	
	GLenum status = glCheckNamedFramebufferStatus(id_, GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_UNDEFINED: { debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_RENDER, "Framebuffer undefined."); } break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: { debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_RENDER, "Framebuffer incomplete attachment."); } break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: { debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_RENDER, "Framebuffer missing attachment."); } break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: { debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_RENDER, "Frambuffer incomplete draw buffer."); } break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: { debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_RENDER, "Framebuffer incomplete read buffer."); } break;
	case GL_FRAMEBUFFER_UNSUPPORTED: { debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_RENDER, "Framebuffer unsupported."); } break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: { debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_RENDER, "Framebuffer incomplete multisample."); } break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: { debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_RENDER, "Framebuffer incomplete layer targets."); } break;
	default: break;
	}

}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::BindColorAttachmentTexture(size_t index)
{
}

void FrameBuffer::UnbindColorAttachmentTexture(size_t index)
{
}

void FrameBuffer::BindDepthAttachmentTexture()
{
}

void FrameBuffer::UnbindDepthAttachmentTexture()
{
}

void FrameBuffer::BindDraw()
{
	GLint current_framebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_framebuffer);
	if (current_framebuffer == 0)
	{
		GLint viewport[4]{};
		glGetIntegerv(GL_VIEWPORT, viewport);
		old_viewport_width_ = viewport[2] - viewport[0];
		old_viewport_height_ = viewport[3] - viewport[1];
	}

	glViewport(0, 0, width_, height_);
	glNamedFramebufferDrawBuffer(id_, GL_COLOR_ATTACHMENT0);
	glNamedFramebufferDrawBuffer(id_, GL_DEPTH_ATTACHMENT);
}

void FrameBuffer::UnbindDraw()
{
	if (old_viewport_width_ != 0 && old_viewport_height_ != 0)
	{
		glViewport(0, 0, old_viewport_width_, old_viewport_height_);
		glNamedFramebufferDrawBuffer(0, GL_BACK);
		glNamedFramebufferReadBuffer(id_, GL_COLOR_ATTACHMENT0);
		glNamedFramebufferReadBuffer(id_, GL_DEPTH_ATTACHMENT);
	}
}

void FrameBuffer::Free()
{
	glDeleteFramebuffers(1, &id_);
	glDeleteTextures(1, &depth_texture_);
	glDeleteTextures(static_cast<GLsizei>(textures_.size()), textures_.data());
}
