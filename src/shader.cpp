#include "shader.h"

#include <functional>

#include "file.h"
#include "logger.h"

using namespace graphics;

ProgramCache::ProgramCache()
{
}

ProgramCache::~ProgramCache()
{
	for (auto &program : programs_)
	{
		glDeleteProgram(program.second.id);
	}
}

const Program & ProgramCache::GetFromFile(const std::string & name, 
	size_t & out_hash, GLenum program_type, const std::string & path)
{
	input::FileReader reader;
	std::string source = reader.ReadTextFile(path);
	const char * source_ptr = source.c_str();

	Program program;
	program.id = glCreateShaderProgramv(program_type, 1, &source_ptr);

	GLint link_status;
	glGetProgramiv(program.id, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE)
	{
		GLint info_log_length;
		glGetProgramiv(program.id, GL_INFO_LOG_LENGTH, &info_log_length);
		if (info_log_length > 0)
		{
			GLchar log[512];
			glGetProgramInfoLog(program.id, info_log_length, 0, log);
			debug::Log(SDL_LOG_PRIORITY_WARN, SDL_LOG_CATEGORY_RENDER, (const char *)log);
		}
	}

	program.flag = program_type;
	program.hash = std::hash<std::string>{}(name);
	programs_.insert({ program.hash, program });

	out_hash = program.hash;

	return program;
}

const Program & ProgramCache::GetFromName(const std::string & name)
{
	return programs_[std::hash<std::string>{}(name)];
}

const Program & ProgramCache::GetFromHash(size_t hash)
{
	return programs_[hash];
}

graphics::ProgramPipeline::ProgramPipeline()
{
	glGenProgramPipelines(1, &id_);
}

graphics::ProgramPipeline::~ProgramPipeline()
{
	glDeleteProgramPipelines(1, &id_);
}

void graphics::ProgramPipeline::Bind()
{
	glBindProgramPipeline(id_);
}

void graphics::ProgramPipeline::Unbind()
{
	glBindProgramPipeline(0);
}

void graphics::ProgramPipeline::SetStages(const Program & program)
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

	glUseProgramStages(id_, flags, program.id);
}
