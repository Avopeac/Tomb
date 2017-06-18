#pragma once

#include <string>
#include <unordered_map>

#include "GL/glew.h"

namespace graphics
{
	struct Program
	{
		GLenum flag;
		GLuint id;
	};

	class ProgramCache
	{

		std::unordered_map<size_t, Program> programs_;

	public:

		ProgramCache();

		~ProgramCache();

		void CompileFromFile(GLenum programType, const std::string &path);

		Program &GetProgramByName(const std::string &name);

		Program &GetProgramByHash(size_t hash);

	};

}