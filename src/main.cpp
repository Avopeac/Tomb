#include "SDL.h"
#include "SDL_image.h"

#include "shader.h"
#include "graphics.h"
#include "texture.h"
#include "logger.h"
#include "timing.h"

Sint32 main(Sint32 argc, char * argv[])
{
	// Initialization
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	SDL_LogSetOutputFunction(debug::LogOutputFunction, 0);

	// Load config
	input::Config config;
	config.Load("assets/config.json");

	// Create window and initialize graphics
	graphics::GraphicsBase graphics_base;
	graphics_base.Initialize(config);
	graphics_base.BindVertexArray();

	graphics::ProgramCache program_cache;
	program_cache.CompileFromFile(GL_VERTEX_SHADER, "assets/shaders/default.vert");
	program_cache.CompileFromFile(GL_FRAGMENT_SHADER, "assets/shaders/default.frag");

	graphics::TextureCache texture_cache;
	texture_cache.CreateFromFile("assets/textures/smiley.png");

	auto smiley_texture = texture_cache.GetTextureFromPath("assets/textures/smiley.png");

	graphics::Sampler sampler;
	sampler.SetFiltering(graphics::MagnificationFiltering::Linear, 
		graphics::MinificationFiltering::Linear);
	

	auto vert_prog = program_cache.GetProgramByName("assets/shaders/default.vert");
	auto frag_prog = program_cache.GetProgramByName("assets/shaders/default.frag");

	graphics_base.SetPipelineStages(vert_prog);
	graphics_base.SetPipelineStages(frag_prog);

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

		sampler.Bind(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, smiley_texture.id);
		glProgramUniform1i(frag_prog.id, glGetUniformLocation(frag_prog.id, "u_texture"), 0);
	
		graphics_base.DrawQuad();

		SDL_GL_SwapWindow(graphics_base.GetWindow());
	}

	graphics_base.Free();

	SDL_Quit();
	IMG_Quit();

	return 0;
}