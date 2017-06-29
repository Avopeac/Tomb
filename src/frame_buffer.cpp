#include "frame_buffer.h"

#include "texture.h"
#include "logger.h"

using namespace graphics;

FrameBuffer::FrameBuffer(Uint32 width, Uint32 height, bool depth, bool dynamic_range_enabled)
	: width_(width), height_(height), depth_texture_(0),
	old_viewport_width_(0), old_viewport_height_(0)
{

	glCreateFramebuffers(1, &id_);

	// Create color attachment
	{
		GLenum internalFormat = dynamic_range_enabled ? GL_RGBA16F : GL_RGBA8;
		GLenum format = dynamic_range_enabled ? GL_FLOAT : GL_UNSIGNED_BYTE;

		GLuint color_attachment;
		glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment);
		glBindTextureUnit(0, color_attachment);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0,
			GL_RGBA, format, nullptr);
		glTextureParameteri(color_attachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(color_attachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(color_attachment, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(color_attachment, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTextureUnit(0, 0);
		textures_.push_back(color_attachment);
		texture_bind_points_.push_back(-1);

		// Attach to framebuffer
		glNamedFramebufferTexture(id_, GL_COLOR_ATTACHMENT0, color_attachment, 0);
	}
	
	// Create depth attachment
	if (depth)
	{
		GLuint depth_attachment;
		glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment);
		glBindTextureUnit(0, depth_attachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width_, height_, 0,
			GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
		glTextureParameteri(depth_attachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(depth_attachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(depth_attachment, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(depth_attachment, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTextureUnit(0, 0);
		
		depth_texture_ = depth_attachment;
		depth_texture_bind_point_ = -1;

		// Attach to framebuffer
		glNamedFramebufferTexture(id_, GL_DEPTH_ATTACHMENT, depth_texture_, 0);
	}

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

void FrameBuffer::BindColorAttachmentTexture(size_t index, GLuint unit)
{
	if (index < textures_.size())
	{
		glBindTextureUnit(unit, textures_[index]);
		texture_bind_points_[index] = unit;
	}
}

void FrameBuffer::UnbindColorAttachmentTexture(size_t index)
{
	if (index < textures_.size())
	{
		glBindTextureUnit(texture_bind_points_[index], textures_[index]);
		texture_bind_points_[index] = -1;
	}
}

void FrameBuffer::BindDepthAttachmentTexture(GLuint unit)
{
	if (depth_texture_ != 0)
	{
		glBindTextureUnit(unit, depth_texture_);
		depth_texture_bind_point_ = unit;
	}
}

void FrameBuffer::UnbindDepthAttachmentTexture()
{
	if (depth_texture_ != 0)
	{
		glBindTextureUnit(depth_texture_bind_point_, 0);
		depth_texture_bind_point_ = -1;
	}
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
	//glNamedFramebufferDrawBuffer(id_, GL_COLOR_ATTACHMENT0);
	//glNamedFramebufferDrawBuffer(id_, GL_DEPTH_ATTACHMENT);
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
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id_);
	}
}

void FrameBuffer::Free()
{
	glDeleteFramebuffers(1, &id_);
	glDeleteTextures(1, &depth_texture_);
	glDeleteTextures(static_cast<GLsizei>(textures_.size()), textures_.data());
}
