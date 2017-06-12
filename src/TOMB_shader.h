#pragma once

#include "GL/glew.h"

#include "TOMB_defines.h"
#include "TOMB_file.h"

struct TOMB_ShaderProgram
{
	GLenum flag;
	GLuint shaderProgramId;
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

	GLint linkStatus;
	glGetProgramiv(pShader->shaderProgramId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) 
	{
		GLint infoLogLength;
		glGetProgramiv(pShader->shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			GLchar * log = (GLchar *)TOMB_MemoryArenaReserve(pArena, (Uint64)infoLogLength);
			glGetProgramInfoLog(pShader->shaderProgramId, infoLogLength, 0, log);

			printf("%s\n", log);

			//SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Could not link shader, log shows: %s.",
				//(const char *)log);

		}
	}

	pShader->flag = shaderProgramType;

	return TOMB_SUCCESS;
}