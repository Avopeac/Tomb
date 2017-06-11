#include <stdio.h>

#include "SDL.h"

#include "GL/glew.h"

#include "TOMB_config.h"
#include "TOMB_defines.h"
#include "TOMB_memory.h"
#include "TOMB_shader.h"
#include "TOMB_graphics.h"

static void TOMB_LogOutputFunction(void * pUserData, Sint32 category, SDL_LogPriority priority, const char * pMessage)
{
	const char * pPriorityString;

	switch (priority)
	{
	case SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL: { pPriorityString = "CRITICAL"; } break;
	case SDL_LogPriority::SDL_LOG_PRIORITY_INFO: { pPriorityString = "INFO"; } break;
	case SDL_LogPriority::SDL_LOG_PRIORITY_WARN: { pPriorityString = "WARNING"; } break;
	case SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE: { pPriorityString = "VERBOSE"; } break;
	case SDL_LogPriority::SDL_LOG_PRIORITY_DEBUG: { pPriorityString = "DEBUG"; } break;
	case SDL_LogPriority::SDL_LOG_PRIORITY_ERROR: { pPriorityString = "ERROR"; } break;
	default: { pPriorityString = "UNDEFINED"; } break;
	}

	const char * pCategoryString;

	switch (category)
	{
	case SDL_LOG_CATEGORY_VIDEO: { pCategoryString = "VIDEO"; } break;
	case SDL_LOG_CATEGORY_ASSERT: { pCategoryString = "ASSERT"; } break;
	case SDL_LOG_CATEGORY_AUDIO: { pCategoryString = "AUDIO"; } break;
	case SDL_LOG_CATEGORY_ERROR: { pCategoryString = "ERROR"; } break;
	case SDL_LOG_CATEGORY_INPUT: { pCategoryString = "INPUT"; } break;
	case SDL_LOG_CATEGORY_RENDER: { pCategoryString = "RENDER"; } break;
	case SDL_LOG_CATEGORY_TEST: { pCategoryString = "TEST"; } break;
	case SDL_LOG_CATEGORY_SYSTEM: { pCategoryString = "SYSTEM"; } break;
	case SDL_LOG_CATEGORY_APPLICATION: { pCategoryString = "APPLICATION"; } break;
	default: { pCategoryString = "UNDEFINED"; } break;
	}

	printf("LOG-%s-%s: %s\n", pPriorityString, pCategoryString, pMessage);
}

Sint32 TOMB_CreateGraphicsWindow(TOMB_Config * pConfig, SDL_Window ** ppWindow, SDL_GLContext * pContext)
{
	Sint32 status = TOMB_SUCCESS;

	// Create window with these attributes

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
	SDL_GL_SetSwapInterval(pConfig->verticalSync);

	*ppWindow = SDL_CreateWindow(pConfig->pWindowTitle,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		pConfig->windowWidth,
		pConfig->windowHeight,
		SDL_WINDOW_OPENGL |
		SDL_WINDOW_RESIZABLE);

	if (!*ppWindow)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
		status = TOMB_FAILURE;
	}

	// Create GL context

	*pContext = SDL_GL_CreateContext(*ppWindow);

	if (!*pContext)
	{
		SDL_DestroyWindow(*ppWindow);
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
		status = TOMB_FAILURE;
	}

	// Make the GL context current to load extensions

	if (SDL_GL_MakeCurrent(*ppWindow, *pContext))
	{
		SDL_GL_DeleteContext(*pContext);
		SDL_DestroyWindow(*ppWindow);
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
		status = TOMB_FAILURE;
	}

	// Load GL extensions

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_GL_DeleteContext(*pContext);
		SDL_DestroyWindow(*ppWindow);
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Could not load GL extensions.");
		status = TOMB_FAILURE;
	}

	return status;
}

double TOMB_GetSeconds()
{
	TOMB_STATIC Uint64 frequency = SDL_GetPerformanceFrequency();
	return (double)SDL_GetPerformanceCounter() / frequency;
}

Sint32 TOMB_Main(Sint32 numArguments, char * pArguments[])
{
	// Initialization
	
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_LogSetOutputFunction(TOMB_LogOutputFunction, 0);

	TOMB_Memory memory;
	memory.PersistentStorageSize = TOMB_GIGABYTES_TO_BYTES(1);
	memory.TransientStorageSize = TOMB_MEGABYTES_TO_BYTES(512);
	memory.pPersistentStorage = SDL_malloc(memory.PersistentStorageSize);
	memory.pTransientStorage = SDL_malloc(memory.TransientStorageSize);

	if (!memory.pPersistentStorage || !memory.pTransientStorage)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not allocate memory.");
		SDL_Quit();
		return TOMB_FAILURE;
	}

	TOMB_MemoryArena inputArena;
	TOMB_MemoryArenaInitialize(&inputArena, 
		(Uint8 *)memory.pTransientStorage,
		TOMB_MEGABYTES_TO_BYTES(10));

	// Load config

	TOMB_Config config;
	const char * pConfigPath = "assets/config.json";
	if (TOMB_ConfigLoadFile(&inputArena, &config, pConfigPath) != TOMB_SUCCESS)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_INPUT, "Could not load config file, aborting.");
		SDL_Quit();
		return TOMB_FAILURE;
	}

	TOMB_MemoryArenaFree(&inputArena, inputArena.Occupied);

	// Create window

	SDL_Window * pWindow = 0;
	SDL_GLContext context = 0;
	if (TOMB_CreateGraphicsWindow(&config, &pWindow, &context) != TOMB_SUCCESS)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "Could not create game window, aborting.");
		SDL_Quit();
		return TOMB_FAILURE;
	}

	TOMB_ShaderProgram vertexShaderProgram;
	TOMB_ShaderProgram fragmentShaderProgram;
	const char * pVertexShaderProgramPath = "assets/shaders/default.vert";
	const char * pFragmentShaderProgramPath = "assets/shaders/default.frag";
	TOMB_ShaderProgramCompileFromFile(&inputArena, &vertexShaderProgram, pVertexShaderProgramPath, GL_VERTEX_SHADER);
	TOMB_ShaderProgramCompileFromFile(&inputArena, &fragmentShaderProgram, pFragmentShaderProgramPath, GL_FRAGMENT_SHADER);

	TOMB_GraphicsBase graphicsBase;
	TOMB_GraphicsBaseInitialize(&graphicsBase);

	// Main loop

	bool running = true;
	double previousTime = TOMB_GetSeconds();
	
	while(running)
	{
		double currentTime = TOMB_GetSeconds();
		double frameTime = 1000.0 * (currentTime - previousTime);
		previousTime = currentTime;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			{
				running = false;
			}
		}

		float red = 0.5f + 0.5f * (float)glm::sin(currentTime);

		glClearColor(red, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_GL_SwapWindow(pWindow);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return TOMB_SUCCESS;
}