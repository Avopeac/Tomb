#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "keymap.h"

#include "graphics.h"
#include "logger.h"
#include "timing.h"
#include "renderer.h"

#include "maplogic.h"
#include "mapmodel.h"
#include "mapview.h"

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

	// TESTING: Map shapes
	game::MapLogic logic;
	game::MapModel model(4, 4, game::MapShapeType::FlatHexagon, logic);
	game::MapView view(model, logic);
	
	// Main loop
	bool running = true;
	double previous_time = util::GetSeconds();
	while(running)
	{
		double current_time = util::GetSeconds();
		double frame_time = current_time - previous_time;
		previous_time = current_time;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			{
				running = false;
			}

			input::Keymap::Get().UpdateCurrentInput(event);
		}

		view.Update(renderer, (float)frame_time);

		//std::string print = "Frame: ";
		//print.append(std::to_string(frame_time));
		//print.append(" ms");

		//std::cout << print << std::endl;
		//print.append("(");
		//print.append(std::to_string(1.0 / frame_time));
		//print.append(" FPS)");

		//renderer.GetFontRenderer().Push(print, glm::ivec4(1, 1, 1, 1), 
			//glm::vec2(20, graphics_base.GetBackbufferHeight() - 32), glm::vec2(32, 32));

		if (input::Keymap::Get().KeyDown(input::Key::KeyK))
		{
			std::cout << "hello" << std::endl;
		}
				 
		renderer.Invoke((float)frame_time);

		SDL_GL_SwapWindow(graphics_base.GetWindow());

		input::Keymap::Get().CarryCurrentInput();
	}

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	
	return 0;
}