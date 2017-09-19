#pragma once

#include <string>
#include <unordered_map>

#include "SDL_assert.h"
#include "GL/glew.h"

#include "logger.h"

namespace graphics
{
	struct ProgramUniformData
	{
		GLint size;
		GLenum type;
		int location;

		// Max amount of data per uniform per respective type
		union
		{
			Sint32 data_int[4];
			Uint32 data_uint[4];
			float data_float[16];
		};
	};

	class Program
	{
		GLenum flag_;
		GLuint id_;
		size_t hash_;

		std::unordered_map<std::string, ProgramUniformData> uniforms_;

	public:

		Program();

		Program(size_t hash, GLuint id, GLenum flag);

		~Program();

		Program(const Program &);

		Program(Program &&);

		Program &operator=(Program &&other)
		{
			if (this == &other)
			{
				SDL_assert(false);
			}

			hash_ = other.hash_;
			id_ = other.id_;
			flag_ = other.flag_;
			uniforms_ = other.uniforms_;

			return *this;
		}

		Program &operator=(const Program &other)
		{
			if (this == &other)
			{
				SDL_assert(false);
			}
				
			hash_ = other.hash_;
			id_ = other.id_;
			flag_ = other.flag_;
			uniforms_ = other.uniforms_;
			
			return *this;
		}

		void SetUniform(const std::string &name, void * data);

		inline GLuint GetId() const { return id_; }

		inline size_t GetHash() const { return hash_; }

		inline GLenum GetFlag() const { return flag_; }

		inline bool HasUniform(const std::string &name) const { return uniforms_.find(name) != uniforms_.end(); }

		inline bool IsValid()
		{
			glValidateProgram(id_);

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

		Program &GetFromFile(const std::string &name, GLenum program_type, const std::string &path, size_t * hash = nullptr);

		Program &GetFromName(const std::string &name);

		Program &GetFromHash(size_t hash);

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