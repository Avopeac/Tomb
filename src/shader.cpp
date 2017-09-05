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
		glDeleteProgram(program.second.GetId());
	}
}

Program & ProgramCache::GetFromFile(const std::string & name, 
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

	out_hash = std::hash<std::string>{}(name);
	programs_.insert({ out_hash, Program(out_hash, program_id, program_type) });
	return programs_[out_hash];
}

Program & ProgramCache::GetFromName(const std::string & name)
{
	return programs_[std::hash<std::string>{}(name)];
}

Program & ProgramCache::GetFromHash(size_t hash)
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

	switch (program.GetFlag())
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

	glUseProgramStages(id_, flags, program.GetId());
}

Program::Program() :
	hash_(0), id_(0), flag_(0)
{
}

Program::Program(size_t hash, GLuint id, GLenum flag) :
	hash_(hash), id_(id), flag_(flag)
{
	int num_uniforms;
	glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &num_uniforms);

	int max_length;
	glGetProgramiv(id_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);

	int length;
	int size;
	GLenum type;
	GLchar * name = (GLchar*)SDL_malloc(max_length);

	for (int i = 0; i < num_uniforms; ++i)
	{
		glGetActiveUniform(id_, i, max_length, &length, &size, &type, name);
		std::string name_str(name);

		uniforms_.insert({ name, ProgramUniformData{} });
		auto &uniform = uniforms_[name];
		uniform.location = i;
		uniform.size = size;
		uniform.type = type;
	}

	SDL_free(name);
}

Program::~Program()
{
}

Program::Program(const Program &other) :
	hash_(other.hash_), id_(other.id_), flag_(other.flag_), uniforms_(other.uniforms_)
{
}

Program::Program(Program &&other) :
	hash_(other.hash_), id_(other.id_), flag_(other.flag_), uniforms_(other.uniforms_)
{
}

void Program::SetUniform(const std::string & name, void * data)
{
	auto &uniform = uniforms_[name];
	
	SDL_assert(data);

	switch (uniform.type)
	{

		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		{
			uniform.data_float[0] = ((float*)data)[0];
			if (uniform.type == GL_FLOAT_VEC2)
				uniform.data_float[1] = ((float*)data)[1];
			if (uniform.type == GL_FLOAT_VEC3)
				uniform.data_float[2] = ((float*)data)[2];
			if (uniform.type == GL_FLOAT_VEC4)
				uniform.data_float[3] = ((float*)data)[3];
		} break;

		case GL_INT:
		case GL_INT_VEC2:
		case GL_INT_VEC3:
		case GL_INT_VEC4:
		{
			uniform.data_int[0] = ((Sint32*)data)[0];
			if (uniform.type == GL_FLOAT_VEC2)
				uniform.data_int[1] = ((Sint32*)data)[1];
			if (uniform.type == GL_FLOAT_VEC3)
				uniform.data_int[2] = ((Sint32*)data)[2];
			if (uniform.type == GL_FLOAT_VEC4)
				uniform.data_int[3] = ((Sint32*)data)[3];
		} break;

		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_VEC2:
		case GL_UNSIGNED_INT_VEC3:
		case GL_UNSIGNED_INT_VEC4:
		{
			uniform.data_uint[0] = ((Uint32*)data)[0];
			if (uniform.type == GL_FLOAT_VEC2)
				uniform.data_uint[1] = ((Uint32*)data)[1];
			if (uniform.type == GL_FLOAT_VEC3)
				uniform.data_uint[2] = ((Uint32*)data)[2];
			if (uniform.type == GL_FLOAT_VEC4)
				uniform.data_uint[3] = ((Uint32*)data)[3];
		} break;

		case GL_FLOAT_MAT2:
		{
			for (int i = 0; i < 4; ++i) 
			{
				uniform.data_float[i] = ((float*)data)[i];
			}
		} break;

		case GL_FLOAT_MAT3:
		{
			for (int i = 0; i < 9; ++i)
			{
				uniform.data_float[i] = ((float*)data)[i];
			}
		} break;

		case GL_FLOAT_MAT4:
		{
			for (int i = 0; i < 16; ++i)
			{
				uniform.data_float[i] = ((float*)data)[i];
			}
		} break;

		// TODO: Add more uniform types!

		default:
		{
			SDL_assert(false);
		}
	}
}
