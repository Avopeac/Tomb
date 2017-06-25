#pragma once

#include "SDL.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "file.h"
#include "shader.h"
#include "config.h"
#include "disposable.h"

namespace graphics
{
	class GraphicsBase : public base::Disposable
	{
		SDL_Window * window_;
		SDL_GLContext context_;

		glm::mat4 projection_;
		glm::mat4 view_;
		glm::mat4 view_projection_;

	public:

		const glm::vec2 quad_vertices[4] =
		{
			glm::vec2(-1.0f, -1.0f),
			glm::vec2(1.0f, -1.0f),
			glm::vec2(-1.0f,  1.0f),
			glm::vec2(1.0f,  1.0f),
		};

		Uint32 quad_indices[6] =
		{
			0, 1, 2, 2, 1, 3
		};


		GraphicsBase();

		~GraphicsBase();

		void Initialize(const input::Config &config);
		
		// Inherited via Disposable
		virtual void Free() override;

		inline SDL_Window * GetWindow() const { return window_; }

		inline glm::mat4 GetProjection() const { return projection_; }

		inline glm::mat4 GetView() const { return view_; }

		inline glm::mat4 GetViewProjection() const { return view_projection_; }

		void Log();

	};
}
