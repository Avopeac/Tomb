#pragma once

#include <string>
#include <unordered_map>

#include "GL/glew.h"

#include "logger.h"
#include "graphics.h"

namespace graphics
{
	struct Program
	{
		GLenum flag;
		GLuint id;
		size_t hash;

		inline bool IsValid()
		{
			glValidateProgram(id);

			GLenum err = glGetError();

			if (err != GL_NO_ERROR)
			{
				debug::Log(SDL_LOG_PRIORITY_DEBUG, 
					SDL_LOG_CATEGORY_RENDER, 
					(const char *)glewGetErrorString(err));
			}

			return err != GL_NO_ERROR;
		}
	};

	class ProgramCache
	{

		std::unordered_map<size_t, Program> programs_;

	public:

		ProgramCache();

		~ProgramCache(); 

		const Program &GetFromFile(const std::string &name, size_t &out_hash, 
			GLenum program_type, const std::string &path);

		const Program &GetFromName(const std::string &name);

		const Program &GetFromHash(size_t hash);

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