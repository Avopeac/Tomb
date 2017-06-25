#include "SDL.h"
#include "SDL_image.h"

#include "glm/gtc/random.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "graphics.h"
#include "texture.h"
#include "logger.h"
#include "timing.h"
#include "sprite_renderer.h"

Sint32 main(Sint32 argc, char * argv[])
{
	// Initialization
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	// Load config
	input::Config config;
	config.Load("assets/config.json");

	// Create window and initialize graphics
	graphics::GraphicsBase graphics_base;
	graphics_base.Initialize(config);
	
	graphics::ProgramCache program_cache;
	graphics::TextureCache texture_cache;

	graphics::SpriteRenderer sprite_renderer(4000, graphics_base, program_cache, texture_cache);
	for (auto i = 0; i < 2000; i++)
	{ 
		graphics::Sprite sprite(glm::translate(glm::scale(glm::mat4(1),
			glm::vec3(128, 128, 128)), glm::vec3(i, 0, 0)));

		sprite.SetLayer(i);
		sprite_renderer.Push(sprite, 0, 0);
	}
	
	// Main loop
	bool running = true;
	double previous_time = util::GetSeconds();
	while(running)
	{
		double current_time = util::GetSeconds();
		double frame_time = 1000.0 * (current_time - previous_time);
		previous_time = current_time;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			{
				running = false;
			}
		}

		float red = 0.5f + 0.5f * (float)glm::sin(current_time);

		glClearColor(red, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sprite_renderer.Draw();

		SDL_GL_SwapWindow(graphics_base.GetWindow());
	
	}

	graphics_base.Free();

	SDL_Quit();
	IMG_Quit();

	return 0;
}