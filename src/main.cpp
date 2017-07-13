#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "glm/gtc/random.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "graphics.h"
#include "texture.h"
#include "sampler.h"
#include "blend_mode.h"
#include "logger.h"
#include "timing.h"
#include "sprite_renderer.h"
#include "renderer.h"

Sint32 main(Sint32 argc, char * argv[])
{
	// Initialization
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	TTF_Init();

	// Load config
	input::Config config;
	config.Load("assets/config.json");

	// Create window and initialize graphics
	graphics::GraphicsBase graphics_base(config);
	graphics::Renderer renderer(&graphics_base);

	auto scale_mat = glm::scale(glm::mat4(1), glm::vec3(32, 32, 1));
	for (auto y = 0; y < 32; ++y)
	{
		for (auto x = 0; x < 32; ++x)
		{
			float x_offset = y % 2 == 0 ? 0.5f : -0.5f;
			auto transform = glm::translate(scale_mat, 
				glm::vec3(x_offset + 2.0f * x, 1.5f * y, 0));

			graphics::Sprite sprite(transform, x + y * 32);
			renderer.GetSpriteRenderer().Push(sprite,
				graphics::SpriteShape::SharpHex,
				"assets/textures/sand/sand_08.png",
				graphics::BlendMode::SrcAlpha,
				graphics::BlendMode::OneMinusSrcAlpha,
				graphics::BlendMode::SrcAlpha,
				graphics::BlendMode::OneMinusSrcAlpha,
				graphics::MagnificationFiltering::Linear,
				graphics::MinificationFiltering::LinearMipmapLinear, 
				graphics::Wrapping::ClampToEdge,
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

		std::string print = "Frame: ";
		print.append(std::to_string(1000.0 / frame_time));
		print.append(" FPS");
		//print.append("(");
		//print.append(std::to_string(1.0 / frame_time));
		//print.append(" FPS)");

		renderer.GetFontRenderer().Push(print, glm::ivec4(1, 1, 1, 1), 
			glm::vec2(20, graphics_base.GetBackbufferHeight() - 32), glm::vec2(32, 32));

		renderer.Invoke();

		SDL_GL_SwapWindow(graphics_base.GetWindow());
	}

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	
	return 0;
}