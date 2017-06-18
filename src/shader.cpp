#include "shader.h"

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
	std::string source = reader.ReadTextFile(path);;

	Program program;
	program.id = glCreateShaderProgramv(program_type, 1, (const GLchar * const *)source.c_str());

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
}

Program & ProgramCache::GetProgramByName(const std::string & name)
{
	// TODO: insert return statement here
	return Program();
}

Program & ProgramCache::GetProgramByHash(size_t hash)
{
	// TODO: insert return statement here
	return Program();
}
