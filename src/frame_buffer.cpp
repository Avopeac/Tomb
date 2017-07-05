#include "frame_buffer.h"

#include "texture.h"
#include "logger.h"

using namespace graphics;

FrameBuffer::FrameBuffer(Uint32 width, Uint32 height, bool depth, bool dynamic_range_enabled)
	: width_(width), height_(height), depth_texture_(0),
	old_viewport_width_(0), old_viewport_height_(0)
{

	// Create color attachment
	GLuint color_attachment = 0;
	{
		GLenum internalFormat = dynamic_range_enabled ? GL_RGBA16F : GL_RGBA8;
		GLenum format = GL_RGBA;
		GLenum type = dynamic_range_enabled ? GL_FLOAT : GL_UNSIGNED_BYTE;

		glGenTextures(1, &color_attachment);
		glBindTexture(GL_TEXTURE_2D, color_attachment);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0,
			format, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		textures_.push_back(color_attachment);
		texture_bind_points_.push_back(-1);
	}
	
	// Create depth attachment
	GLuint depth_attachment = 0;
	if (depth)
	{
		GLenum internalFormat = GL_DEPTH_COMPONENT16;
		GLenum format = GL_DEPTH_COMPONENT;
		GLenum type = GL_UNSIGNED_INT;

		glGenTextures(1, &depth_attachment);
		glBindTexture(GL_TEXTURE_2D, depth_attachment);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0,
			format, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		depth_texture_ = depth_attachment;
		depth_texture_bind_point_ = -1;
	}

	glGenFramebuffers(1, &id_);
	glBindFramebuffer(GL_FRAMEBUFFER, id_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment, 0);
	if (depth)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment, 0);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

void FrameBuffer::BindColorAttachmentTexture(size_t index, GLuint unit)
{
	if (index < textures_.size())
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, textures_[index]);
		texture_bind_points_[index] = unit;
	}
}

void FrameBuffer::UnbindColorAttachmentTexture(size_t index)
{
	if (index < textures_.size())
	{
		glActiveTexture(GL_TEXTURE0 + texture_bind_points_[index]);
		glBindTexture(GL_TEXTURE_2D, 0);
		texture_bind_points_[index] = -1;
	}
}

void FrameBuffer::BindDepthAttachmentTexture(GLuint unit)
{
	if (depth_texture_ != 0)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, unit);
		depth_texture_bind_point_ = unit;
	}
}

void FrameBuffer::UnbindDepthAttachmentTexture()
{
	if (depth_texture_ != 0)
	{
		glActiveTexture(GL_TEXTURE0 + depth_texture_bind_point_);
		glBindTexture(GL_TEXTURE_2D, 0);
		depth_texture_bind_point_ = -1;
	}
}

void FrameBuffer::Blit(Sint32 sx0, Sint32 sx1, Sint32 sy0, Sint32 sy1, 
	Sint32 dx0, Sint32 dx1, Sint32  dy0, Sint32 dy1)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_BACK);

	glBlitFramebuffer(sx0, sy0, sx1, sy1, dx0, dy0, dx1, dy1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
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
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id_);
}

void FrameBuffer::UnbindDraw()
{
	if (old_viewport_width_ != 0 && old_viewport_height_ != 0)
	{
		glViewport(0, 0, old_viewport_width_, old_viewport_height_);
		//glNamedFramebufferDrawBuffer(0, GL_BACK);
		//glNamedFramebufferReadBuffer(id_, GL_COLOR_ATTACHMENT0);
		//glNamedFramebufferReadBuffer(id_, GL_DEPTH_ATTACHMENT);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
}

void FrameBuffer::Free()
{
	glDeleteFramebuffers(1, &id_);
	glDeleteTextures(1, &depth_texture_);
	glDeleteTextures(static_cast<GLsizei>(textures_.size()), textures_.data());
}
