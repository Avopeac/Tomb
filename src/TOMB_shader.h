#pragma once

#include "GL/glew.h"

#include "TOMB_defines.h"
#include "TOMB_file.h"

struct TOMB_ShaderProgram
{
	GLuint shaderProgramId;
};

struct TOMB_ProgramPipeline
{
	GLuint programPipelineId;
};

Uint32 TOMB_ShaderProgramCompileFromFile(TOMB_MemoryArena * pArena, TOMB_ShaderProgram * pShader, const char * pPath, GLenum shaderProgramType)
{
	Sint64 size;
	const char * pSource = TOMB_TextReadFile(pArena, pPath, &size);
	if (!pSource)
	{
		return TOMB_FAILURE;
	}

	pShader->shaderProgramId = glCreateShaderProgramv(shaderProgramType, 1, &pSource);
	if (!glIsProgram(pShader->shaderProgramId))
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			const GLubyte * pError = glewGetErrorString(glGetError());
			SDL_LogError(SDL_LOG_CATEGORY_RENDER,
				"GL got the following error on creating shader program: %s.", 
				pError);
		}

		GLsizei logLength;
		GLchar log[512];
		glGetProgramInfoLog(pShader->shaderProgramId, 512, &logLength, log);
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Could not load shader from %s, log shows %s.", pPath, log);

		return TOMB_FAILURE;
	
	}


	return TOMB_SUCCESS;
}