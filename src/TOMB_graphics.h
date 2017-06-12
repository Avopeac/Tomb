#pragma once

#include "SDL.h"

#include "GL/glew.h"

#include "glm/glm.hpp"

struct TOMB_GraphicsBase
{
	float quadVertexData[8] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,
	};

	Uint32 quadIndexData[6] = {
		0, 1, 2, 2, 1, 3,
	};

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint indexBuffer;

	GLuint programPipeline;
};

void TOMB_GraphicsPrintError() 
{
	SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, (const char *)glewGetErrorString(glGetError()));
}

void TOMB_GraphicsBaseInitialize(TOMB_GraphicsBase * pGraphicsBase)
{

	glGenBuffers(1, &pGraphicsBase->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pGraphicsBase->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 
		sizeof(pGraphicsBase->quadVertexData), 
		pGraphicsBase->quadVertexData, 
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &pGraphicsBase->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pGraphicsBase->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(pGraphicsBase->quadIndexData),
		pGraphicsBase->quadIndexData, 
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &pGraphicsBase->vertexArray);
	glBindVertexArray(pGraphicsBase->vertexArray);
	{
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, pGraphicsBase->vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pGraphicsBase->indexBuffer);
	}
	glBindVertexArray(0);

	glGenProgramPipelines(1, &pGraphicsBase->programPipeline);

	// Prevent override from old API
	glUseProgram(0);

	glBindProgramPipeline(pGraphicsBase->programPipeline);

	TOMB_GraphicsPrintError();
}

void TOMB_GraphicsBaseSetPipelineStage(TOMB_GraphicsBase * pGraphicsBase, TOMB_ShaderProgram * pShaderProgram)
{

	GLbitfield flagBits;
	switch (pShaderProgram->flag)
	{
	case GL_VERTEX_SHADER: { flagBits = GL_VERTEX_SHADER_BIT; } break;
	case GL_FRAGMENT_SHADER: { flagBits = GL_FRAGMENT_SHADER_BIT; } break;
	case GL_GEOMETRY_SHADER: { flagBits = GL_GEOMETRY_SHADER_BIT; } break;
	case GL_TESS_CONTROL_SHADER: { flagBits = GL_TESS_CONTROL_SHADER_BIT; } break;
	case GL_TESS_EVALUATION_SHADER_BIT: { flagBits = GL_TESS_EVALUATION_SHADER_BIT; } break;
	case GL_COMPUTE_SHADER_BIT: { flagBits = GL_COMPUTE_SHADER_BIT; } break;
	default: SDL_assert(false);
	}

	glUseProgramStages(pGraphicsBase->programPipeline, flagBits, pShaderProgram->shaderProgramId);
}

void TOMB_GraphicsBaseBindVao(TOMB_GraphicsBase * pGraphicsBase)
{
	glBindVertexArray(pGraphicsBase->vertexArray);
}

void TOMB_GraphicsBaseDrawQuad(TOMB_GraphicsBase * pGraphicsBase)
{
	const GLsizei indexCount = sizeof(pGraphicsBase->quadIndexData) /
		sizeof(pGraphicsBase->quadIndexData[0]);

	glDrawElements(GL_TRIANGLES,
		indexCount,
		GL_UNSIGNED_INT, 
		pGraphicsBase->quadIndexData);
}

void TOMB_GraphicsBaseDelete(TOMB_GraphicsBase * pGraphicsBase)
{
	glDeleteVertexArrays(1, &pGraphicsBase->vertexArray);
	glDeleteBuffers(1, &pGraphicsBase->vertexBuffer);
	glDeleteBuffers(1, &pGraphicsBase->indexBuffer);
}
