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
		size_t hash;
	};

	class ProgramCache
	{

		std::unordered_map<size_t, Program> programs_;

	public:

		ProgramCache();

		~ProgramCache();

		Program CompileFromFile(GLenum programType, const std::string &path);

		const Program &GetProgramByName(const std::string &name);

		const Program &GetProgramByHash(size_t hash);

	};

	class ProgramPipeline
	{

		GLuint id_;

	public:

		ProgramPipeline();

		~ProgramPipeline();

		void Bind();

		void Unbind();

		void SetStages(const Program &program);

	};

}