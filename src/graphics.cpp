#include "graphics.h"

#include "SDL.h"

#include "GL/glew.h"

using namespace graphics;

GraphicsBase::GraphicsBase()
{
}

GraphicsBase::~GraphicsBase()
{
}

void GraphicsBase::Initialize(const input::Config & config)
{
	// Create window with these attributes

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
	SDL_GL_SetSwapInterval(config.GetVerticalSync());

	window_ = SDL_CreateWindow(config.GetWindowTitle().c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		config.GetWindowWidth(),
		config.GetWindowHeight(),
		SDL_WINDOW_OPENGL |
		SDL_WINDOW_RESIZABLE);

	if (!window_)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
	}

	// Create GL context

	context_ = SDL_GL_CreateContext(window_);

	if (!context_)
	{
		SDL_DestroyWindow(window_);
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
	}

	// Make the GL context current to load extensions

	if (SDL_GL_MakeCurrent(window_, context_))
	{
		SDL_GL_DeleteContext(context_);
		SDL_DestroyWindow(window_);
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
	}

	// Get rid of "no context set as current" error

	SDL_ClearError();

	// Load GL extensions

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_GL_DeleteContext(context_);
		SDL_DestroyWindow(window_);
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Could not load GL extensions.");
	}

	// Generate GL objects

	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glGenBuffers(1, &ebo_);

	// Bind VAO

	glBindVertexArray(vao_);

	// Bind VBO to VAO and upload data

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);

	// Bind EBO to VAO and upload data

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);

	// Enable position attribute for each vertex

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Vertex::position));

	// Unbind VAO (not EBO and VBO)

	glBindVertexArray(0);

	// Prevent override from old API

	glUseProgram(0);
	glGenProgramPipelines(1, &pipeline_);
	glBindProgramPipeline(pipeline_);

	Log();
}

void GraphicsBase::SetPipelineStages(Program program)
{

	GLbitfield flags;
	switch (program.flag)
	{
	case GL_VERTEX_SHADER: { flags = GL_VERTEX_SHADER_BIT; } break;
	case GL_FRAGMENT_SHADER: { flags = GL_FRAGMENT_SHADER_BIT; } break;
	case GL_GEOMETRY_SHADER: { flags = GL_GEOMETRY_SHADER_BIT; } break;
	case GL_TESS_CONTROL_SHADER: { flags = GL_TESS_CONTROL_SHADER_BIT; } break;
	case GL_TESS_EVALUATION_SHADER_BIT: { flags = GL_TESS_EVALUATION_SHADER_BIT; } break;
	case GL_COMPUTE_SHADER_BIT: { flags = GL_COMPUTE_SHADER_BIT; } break;
	default: SDL_assert(false);
	}

	glUseProgramStages(pipeline_, flags, program.id);
}

void GraphicsBase::BindVertexArray()
{
	glBindVertexArray(vao_);
}

void GraphicsBase::DrawQuad()
{
	const GLsizei indexCount = sizeof(indices_) / sizeof(indices_[0]);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void GraphicsBase::Free()
{
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);
	SDL_GL_DeleteContext(context_);
	SDL_DestroyWindow(window_);
}

void GraphicsBase::Log()
{
	SDL_LogWarn(SDL_LOG_CATEGORY_RENDER,
		(const char *)glewGetErrorString(glGetError()));
}
