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

	GLuint program_id = 0;
	GLuint shader_id = glCreateShader(program_type);
	GLchar log[512];

	if (shader_id)
	{
		glShaderSource(shader_id, 1, &source_ptr, 0);
		glCompileShader(shader_id);

		program_id = glCreateProgram();
		if (program_id)
		{
			glProgramParameteri(program_id, GL_PROGRAM_SEPARABLE, GL_TRUE);

			GLint compiled = GL_FALSE;
			glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
			if (compiled)
			{
				glAttachShader(program_id, shader_id);
				glLinkProgram(program_id);

				GLint linked = GL_FALSE;
				glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
				if (linked)
				{
					GLint num_attached = 0;
					glGetProgramiv(program_id, GL_ATTACHED_SHADERS, &num_attached);

					std::string log { 
						"Successfully linked and compiled shader " + 
						name +
						" #attachments = " +
						std::to_string(num_attached) 
					};

					debug::Log(SDL_LOG_PRIORITY_VERBOSE, SDL_LOG_CATEGORY_RENDER, log.c_str());
				}
				else
				{
					GLint log_len = 0;
					glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_len);
					if (log_len)
					{
						glGetProgramInfoLog(shader_id, log_len, 0, log);
						debug::Log(SDL_LOG_PRIORITY_WARN, SDL_LOG_CATEGORY_RENDER, (const char *)log);
					}
				}

				glDetachShader(program_id, shader_id);
			}
			else
			{
				GLint log_len = 0;
				glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_len);
				if (log_len)
				{
					glGetShaderInfoLog(shader_id, log_len, 0, log);
					debug::Log(SDL_LOG_PRIORITY_WARN, SDL_LOG_CATEGORY_RENDER, (const char *)log);
				}
			}
		}

		glDeleteShader(shader_id);
	}

	Program program;
	program.id = program_id;
	program.hash = std::hash<std::string>{}(name);
	program.flag = program_type;
	programs_.insert({ program.hash, program });
	out_hash = program.hash;
	
	/*Program program;
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

	out_hash = program.hash;*/

	return programs_[out_hash];
}

const Program & ProgramCache::GetFromName(const std::string & name)
{
	return programs_[std::hash<std::string>{}(name)];
}

const Program & ProgramCache::GetFromHash(size_t hash)
{
	return programs_[hash];
}

ProgramPipeline::ProgramPipeline()
{
	glGenProgramPipelines(1, &id_);
}

ProgramPipeline::~ProgramPipeline()
{
	glDeleteProgramPipelines(1, &id_);
}

void ProgramPipeline::Bind()
{
	glBindProgramPipeline(id_);
}

void ProgramPipeline::Unbind()
{
	glBindProgramPipeline(0);
}

void ProgramPipeline::SetStages(const Program & program)
{
	GLbitfield flags;

	switch (program.flag)
	{
		case GL_VERTEX_SHADER: 
		{ 
			flags = GL_VERTEX_SHADER_BIT; 
		} break;

		case GL_FRAGMENT_SHADER: 
		{ 
			flags = GL_FRAGMENT_SHADER_BIT;
		} break;

		case GL_GEOMETRY_SHADER: 
		{ 
			flags = GL_GEOMETRY_SHADER_BIT; 
		} break;

		case GL_TESS_CONTROL_SHADER: 
		{ 
			flags = GL_TESS_CONTROL_SHADER_BIT; 
		} break;

		case GL_TESS_EVALUATION_SHADER_BIT: 
		{ 
			flags = GL_TESS_EVALUATION_SHADER_BIT; 
		} break;

		case GL_COMPUTE_SHADER_BIT: 
		{
			flags = GL_COMPUTE_SHADER_BIT; 
		} break;

		default: 
		{
			SDL_assert(false);
		}
	}

	glUseProgramStages(id_, flags, program.id);
}
