#pragma once

#include "SDL.h"

#include "GL/glew.h"

#include "glm/glm.hpp"

struct TOMB_GraphicsBase
{
	GLuint VertexArray;
	GLuint VertexBuffer;
};

void TOMB_GraphicsPrintError() 
{
	SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, (const char *)glewGetErrorString(glGetError()));
}

void TOMB_GraphicsBaseInitialize(TOMB_GraphicsBase * pGraphicsBase)
{

	glGenBuffers(1, &pGraphicsBase->VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pGraphicsBase->VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), 0, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &pGraphicsBase->VertexArray);
	glBindVertexArray(pGraphicsBase->VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, pGraphicsBase->VertexArray);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	TOMB_GraphicsPrintError();
}

void TOMB_GraphicsBaseDrawQuad(TOMB_GraphicsBase * pGraphicsBase)
{
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void TOMB_GraphicsBaseDelete(TOMB_GraphicsBase * pGraphicsBase)
{
	glDeleteVertexArrays(1, &pGraphicsBase->VertexArray);
	glDeleteBuffers(1, &pGraphicsBase->VertexBuffer);
}



