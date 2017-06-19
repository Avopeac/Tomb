#pragma once

#include "SDL.h"

#include "glm/glm.hpp"

#include "file.h"
#include "shader.h"
#include "config.h"
#include "disposable.h"

namespace graphics
{

	struct Vertex
	{
		glm::vec2 position;
	};

	class GraphicsBase : public base::Disposable
	{
		Vertex vertices_[4] =
		{
			glm::vec2(-1.0f, -1.0f),
			glm::vec2(1.0f, -1.0f),
			glm::vec2(-1.0f,  1.0f),
			glm::vec2(1.0f,  1.0f),
		};

		Uint32 indices_[6] =
		{
			0, 1, 2, 2, 1, 3
		};

		SDL_Window * window_;
		SDL_GLContext context_;

		GLuint vao_, vbo_, ebo_;
		GLuint pipeline_;

	public:

		GraphicsBase();

		~GraphicsBase();

		void Initialize(const input::Config &config);

		void SetPipelineStages(Program program);

		void BindVertexArray();

		void DrawQuad();

		// Inherited via Disposable
		virtual void Free() override;

		inline SDL_Window * GetWindow() const { return window_; }

	private:

		void Log();

	};
}
