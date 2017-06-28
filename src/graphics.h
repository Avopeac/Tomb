#pragma once

#include "SDL.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "config.h"
#include "disposable.h"

namespace graphics
{
	class GraphicsBase : public base::Disposable
	{
		SDL_Window * window_;
		SDL_GLContext context_;

		Uint32 width_;
		Uint32 height_;

		glm::mat4 projection_;
		glm::mat4 view_;
		glm::mat4 view_projection_;

	public:

		const glm::vec2 hexagon_vertices[7] =
		{

			// Sharp bottom
			glm::vec2(0.0f, 1.0f), // Top vertex
			glm::vec2(1.0f, 0.5f), // Top right vertex
			glm::vec2(1.0f, -0.5f), // Bottom right vertex
			glm::vec2(0.0f, -1.0f), // Bottom vertex
			glm::vec2(-1.0f, -0.5f), // Bottom left vertex
			glm::vec2(-1.0f, 0.5f), // Top left vertex
			glm::vec2(0.0f, 0.0f), // Center vertex

			// Flat bottom
			//glm::vec2(-1.0f, 0.0f), // Left vertex
			//glm::vec2(-0.5f, 1.0f), // Top left vertex
			//glm::vec2(0.5f, 1.0f), // Top right vertex
			//glm::vec2(1.0f, 0.0f), // Right vertex
			//glm::vec2(0.5f, -1.0f), // Bottom right vertex
			//glm::vec2(-0.5f, -1.0f), // Bottom left vertex
			//glm::vec2(0.0f, 0.0f), // Center vertex
		};

		const Uint32 hexagon_indices[18] = {
			0, 7, 1,
			1, 7, 2,
			2, 7, 3,
			3, 7, 4,
			4, 7, 5,
			5, 7, 0,
		};

		const glm::vec2 quad_vertices[4] =
		{
			glm::vec2(-1.0f, -1.0f),
			glm::vec2(1.0f, -1.0f),
			glm::vec2(-1.0f,  1.0f),
			glm::vec2(1.0f,  1.0f),
		};

		const Uint32 quad_indices[6] =
		{
			0, 1, 2,
			2, 1, 3
		};


		GraphicsBase();

		~GraphicsBase();

		void Initialize(const input::Config &config);
		
		// Inherited via Disposable
		virtual void Free() override;

		inline SDL_Window * GetWindow() const { return window_; }

		inline Uint32 GetBackbufferWidth() const { return width_; }

		inline Uint32 GetBackbufferHeight() const { return height_; }

		inline glm::mat4 GetProjection() const { return projection_; }

		inline glm::mat4 GetView() const { return view_; }

		inline glm::mat4 GetViewProjection() const { return view_projection_; }

		void Log();

	};
}
