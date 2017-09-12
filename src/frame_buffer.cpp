#include "frame_buffer.h"

#include "texture.h"
#include "logger.h"

using namespace graphics;

FrameBuffer::FrameBuffer(Uint32 width, Uint32 height, Uint32 num_samples,
	const std::vector<FrameBufferAttachmentDescriptor> &descriptors,
	const FrameBufferAttachmentDescriptor * const depth_stencil_descriptor)
	: width_(width), height_(height), num_samples_(num_samples),
	old_viewport_width_(0), old_viewport_height_(0),
	depth_stencil_attachment_(nullptr)
{

	GLenum target = num_samples_ > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	// Enable multisampling for GL state machine
	if (num_samples_ > 0 && glIsEnabled(GL_MULTISAMPLE) == GL_FALSE)
	{
		glEnable(GL_MULTISAMPLE);
	}

	for (const auto &descriptor : descriptors)
	{
		attachments_.push_back(std::make_unique<FrameBufferAttachment>(width_, height_, num_samples_,
			descriptor));
	}

	if (depth_stencil_descriptor)
	{
		depth_stencil_attachment_ = std::make_unique<FrameBufferAttachment>(width_, height_,
			num_samples_, *depth_stencil_descriptor);
	}

	glGenFramebuffers(1, &id_);
	glBindFramebuffer(GL_FRAMEBUFFER, id_);

	std::vector<GLenum> attachment_enums{};
	for (int i = 0; i < attachments_.size(); ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
			target, attachments_[i]->GetId(), 0);

		attachment_enums.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	
	// TODO: No support for stencil buffer attachments yet.
	// Check requirements of attachment via the format passed in.
	if (depth_stencil_attachment_)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
			target, depth_stencil_attachment_->GetId(), 0);
	}

	if (attachment_enums.size() > 1)
	{
		glDrawBuffers((GLsizei)attachment_enums.size(), attachment_enums.data());
	}
	else
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
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

void FrameBuffer::BindDraw(GLbitfield clear_flags, float r, float g, float b, float a)
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

	if (clear_flags != 0)
	{
		glClearColor(r, g, b, a);
		glClear(clear_flags);
	}
}

void FrameBuffer::UnbindDraw()
{
	if (old_viewport_width_ != 0 && old_viewport_height_ != 0)
	{
		glViewport(0, 0, old_viewport_width_, old_viewport_height_);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
}

void FrameBuffer::Free()
{
	glDeleteFramebuffers(1, &id_);
}

FrameBufferAttachment::FrameBufferAttachment(uint32_t width, 
	uint32_t height, 
	uint32_t num_samples,
	const FrameBufferAttachmentDescriptor &descriptor) :
	width_(width), 
	height_(height), 
	num_samples_(num_samples),
	descriptor_(descriptor)
{
	GLenum target = num_samples_ > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	glGenTextures(1, &id_);
	glBindTexture(target, id_);

	if (num_samples_ > 0)
	{
		glTexImage2DMultisample(target, num_samples_, descriptor_.internal_format, width_, height_, false);
	}
	else
	{
		glTexImage2D(target, 0, descriptor_.internal_format, width_, height_, 
			0, descriptor_.format, descriptor_.type, nullptr);

		// Default texture sampler parameters (best to use sampler objects instead of relying on this)
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glBindTexture(target, 0);

	bind_point_ = -1;
}

FrameBufferAttachment::~FrameBufferAttachment()
{
}

void FrameBufferAttachment::Free()
{
	glDeleteTextures(1, &id_);
}

void FrameBufferAttachment::Bind(Uint32 bind_point)
{
	if (bind_point_ != -1)
	{
		Unbind();
	}

	glActiveTexture(GL_TEXTURE0 + bind_point);
	glBindTexture(num_samples_ > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, id_);
	bind_point_ = bind_point;
}

void FrameBufferAttachment::Unbind()
{
	if (bind_point_ == -1)
	{
		return;
	}

	glActiveTexture(GL_TEXTURE0 + bind_point_);
	glBindTexture(num_samples_ > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 0);
	bind_point_ = -1;
}

FrameBufferCache::FrameBufferCache()
{
}

FrameBufferCache::~FrameBufferCache()
{
	for (auto &buffer : buffers_)
	{
		buffer.second->Free();
	}
}

FrameBuffer & FrameBufferCache::GetFromParameters(const std::string & name, size_t & out_hash, 
	Uint32 width, Uint32 height, Uint32 num_samples, 
	const std::vector<FrameBufferAttachmentDescriptor>& descriptors, 
	const FrameBufferAttachmentDescriptor * const depth_stencil_descriptor)
{
	SDL_assert(!name.empty());

	out_hash = std::hash<std::string>{}(name);

	if (buffers_.find(out_hash) != buffers_.end()) 
	{
		return *buffers_[out_hash]; 
	} 

	buffers_[out_hash] = std::make_unique<FrameBuffer>(width, height, num_samples,
		descriptors, depth_stencil_descriptor);

	return *buffers_[out_hash];
}

FrameBuffer & FrameBufferCache::GetFromName(const std::string & name)
{
	size_t hash = std::hash<std::string>{}(name);
	return *buffers_[hash];
}

FrameBuffer & FrameBufferCache::GetFromHash(size_t hash)
{
	return *buffers_[hash];
}
