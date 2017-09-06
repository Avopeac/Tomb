#pragma once

#include "SDL.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "config.h"

namespace graphics
{
	class GraphicsBase
	{
		SDL_Window * window_;
		SDL_GLContext context_;

		Uint32 width_;
		Uint32 height_;
		
		float ortho_near_;
		float ortho_far_;
		glm::mat4 ortho_proj_;
		glm::mat4 ortho_view_;
		glm::mat4 ortho_view_proj_;

		float persp_near_;
		float persp_far_;
		glm::mat4 persp_proj_;
		glm::mat4 persp_view_;
		glm::mat4 persp_view_proj_;

	public:

		const glm::vec2 sharp_hexagon_vertices[7] =
		{
			glm::vec2(0.0f, 1.0f), // Top vertex
			glm::vec2(1.0f, 0.5f), // Top right vertex
			glm::vec2(1.0f, -0.5f), // Bottom right vertex
			glm::vec2(0.0f, -1.0f), // Bottom vertex
			glm::vec2(-1.0f, -0.5f), // Bottom left vertex
			glm::vec2(-1.0f, 0.5f), // Top left vertex
			glm::vec2(0.0f, 0.0f), // Center vertex
		};

		const glm::vec2 flat_hexagon_vertices[7] =
		{
			glm::vec2(-1.0f, 0.0f), // Left vertex
			glm::vec2(-0.5f, 1.0f), // Top left vertex
			glm::vec2(0.5f, 1.0f), // Top right vertex
			glm::vec2(1.0f, 0.0f), // Right vertex
			glm::vec2(0.5f, -1.0f), // Bottom right vertex
			glm::vec2(-0.5f, -1.0f), // Bottom left vertex
			glm::vec2(0.0f, 0.0f), // Center vertex
		};

		const Uint32 hexagon_indices[18] = {
			0, 6, 1,
			1, 6, 2,
			2, 6, 3,
			3, 6, 4,
			4, 6, 5,
			5, 6, 0,
		};

		const glm::vec2 quad_vertices[4] =
		{
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f,  1.0f),
			glm::vec2(1.0f,  1.0f),
		};

		const Uint32 quad_indices[6] =
		{
			0, 1, 2,
			2, 1, 3
		};

		GraphicsBase(const input::Config &config);

		~GraphicsBase();

		inline SDL_Window * GetWindow() const { return window_; }

		inline Uint32 GetBackbufferWidth() const { return width_; }

		inline Uint32 GetBackbufferHeight() const { return height_; }

		inline glm::mat4 GetOrthoProj() const { return ortho_proj_; }

		inline glm::mat4 GetOrthoView() const { return ortho_view_; }

		inline glm::mat4 GetOrthoViewProj() const { return ortho_view_proj_; }

		inline glm::mat4 GetPerspProj() const { return persp_proj_; }

		inline glm::mat4 GetPerspView() const { return persp_view_; } 
		 
		inline glm::mat4 GetPerspViewProj() const { return persp_view_proj_; }

		void Log();

	};
}
