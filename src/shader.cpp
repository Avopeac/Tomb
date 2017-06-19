#include "shader.h"

#include <functional>

#include "file.h"


using namespace graphics;

ProgramCache::ProgramCache()
{
}

ProgramCache::~ProgramCache()
{
}

void ProgramCache::CompileFromFile(GLenum program_type, const std::string &path)
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

			SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Could not link shader, log shows: %s.", (const char *) log);

		}
	}

	program.flag = program_type;

	programs_.insert({ std::hash<std::string>{}(path), program });
}

Program & ProgramCache::GetProgramByName(const std::string & name)
{
	return programs_[std::hash<std::string>{}(name)];
}

Program & ProgramCache::GetProgramByHash(size_t hash)
{
	return programs_[hash];
}
