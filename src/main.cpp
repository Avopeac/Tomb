#include "SDL.h"
#include "SDL_image.h"

#include "glm/gtc/random.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "graphics.h"
#include "texture.h"
#include "sampler.h"
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
	graphics::SamplerCache sampler_cache;

	graphics::SpriteRenderer sprite_renderer(4000, graphics_base, program_cache, 
		texture_cache, sampler_cache);


	auto scale_mat = glm::scale(glm::mat4(1), glm::vec3(32, 32, 1));
	for (auto y = 0; y < 32; ++y)
	{
		for (auto x = 0; x < 32; ++x)
		{
			float x_offset = y % 2 == 0 ? 0.5f : -0.5f;
			auto transform = glm::translate(scale_mat, glm::vec3(x_offset + 2.1f * x, 1.6f * y, 0));

			graphics::Sprite sprite(transform);

			size_t texture_hash;
			texture_cache.GetFromFile(texture_hash, "assets/textures/sand/sand_08.png");
			sprite.SetTexture(texture_hash);
			sprite.SetLayer(x + y * 32);
			sprite_renderer.Push(sprite, 0, graphics::MagnificationFiltering::Linear,
				graphics::MinificationFiltering::LinearMipmapLinear, graphics::Wrapping::ClampToEdge,
				graphics::Wrapping::ClampToEdge);
		}
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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sprite_renderer.Draw();

		SDL_GL_SwapWindow(graphics_base.GetWindow());
	}

	graphics_base.Free();

	SDL_Quit();
	IMG_Quit();

	return 0;
}