#include "graphics.h"

#include "SDL.h"

#include "GL/glew.h"

#include "logger.h"

using namespace graphics;

GraphicsBase::GraphicsBase(const input::Config & config)
{
	// Create window with these attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4); 
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetSwapInterval(config.GetVerticalSync());

	window_ = SDL_CreateWindow(config.GetWindowTitle().c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		config.GetWindowWidth(),
		config.GetWindowHeight(),
		SDL_WINDOW_OPENGL);

	if (!window_)
	{
		debug::Log(SDL_LOG_PRIORITY_CRITICAL, SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
	}

	// Create GL context

	context_ = SDL_GL_CreateContext(window_);

	if (!context_)
	{
		SDL_DestroyWindow(window_);

		debug::Log(SDL_LOG_PRIORITY_CRITICAL, SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
	}

	// Make the GL context current to load extensions

	if (SDL_GL_MakeCurrent(window_, context_))
	{
		SDL_GL_DeleteContext(context_);
		SDL_DestroyWindow(window_);

		debug::Log(SDL_LOG_PRIORITY_CRITICAL, SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
	}

	// Get rid of "no context set as current" error

	SDL_ClearError();

	// Load GL extensions

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_GL_DeleteContext(context_);
		SDL_DestroyWindow(window_);

		debug::Log(SDL_LOG_PRIORITY_CRITICAL, SDL_LOG_CATEGORY_VIDEO, "Could not load GL extensions.");
	}

	width_ = config.GetWindowWidth();
	height_ = config.GetWindowHeight();
	near_ = 0.0f;
	far_ = 1.0f;

	projection_ = glm::ortho(
		0.0f, (float)config.GetWindowWidth(),
		0.0f, (float)config.GetWindowHeight(),
		near_, far_
	);

	// TODO: View matrix should be manipulated by camera later on
	view_ = glm::mat4(1);
	view_projection_ = projection_ * view_;
}

GraphicsBase::~GraphicsBase()
{
	SDL_GL_DeleteContext(context_);
	SDL_DestroyWindow(window_);
}

void GraphicsBase::Log()
{
	debug::Log(SDL_LOG_PRIORITY_INFO, SDL_LOG_CATEGORY_RENDER, (const char *)glewGetErrorString(glGetError()));
}
