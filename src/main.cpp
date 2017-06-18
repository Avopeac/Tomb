#include "memory.h"
#include "shader.h"
#include "graphics.h"
#include "logger.h"
#include "timing.h"

Sint32 main(Sint32 numArguments, char * pArguments[])
{
	// Initialization
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_LogSetOutputFunction(debug::LogOutputFunction, 0);

	// Load config
	input::Config config;
	config.Load("assets/config.json");

	// Create window and initialize graphics
	graphics::GraphicsBase graphics_base;
	graphics_base.Initialize(config);

	// Main loop
	bool running = true;
	double previousTime = util::GetSeconds();
	while(running)
	{
		double currentTime = util::GetSeconds();
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

		SDL_GL_SwapWindow(graphics_base.GetWindow());
	}

	graphics_base.Free();

	SDL_Quit();

	return 0;
}