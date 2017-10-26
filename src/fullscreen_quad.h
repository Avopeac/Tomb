#pragma once

#include "SDL.h"

#include "glm/glm.hpp"

#include "GL/glew.h"

namespace graphics
{
	class FullscreenQuad
	{
		const glm::vec2 quad_vertices_[4] =
		{
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f,  1.0f),
			glm::vec2(1.0f,  1.0f),
		};

		const Uint32 quad_indices_[6] =
		{
			0, 1, 2,
			2, 1, 3
		};

		GLuint vao_, vbo_, ebo_;

	public:

		static FullscreenQuad &Get()
		{
			static FullscreenQuad quad;
			return quad;
		}

		~FullscreenQuad() = default;

		inline Uint32 GetNumQuadVertices() const { return sizeof(quad_vertices_) / sizeof(quad_vertices_[0]); }

		inline Uint32 GetNumQuadIndices() const { return sizeof(quad_indices_) / sizeof(quad_indices_[0]); }

		inline const Uint32 * const GetQuadIndices() const { return quad_indices_; }

		inline const glm::vec2 * const GetQuadVertices() const { return quad_vertices_; }

		inline void Begin() const { glBindVertexArray(vao_); }

		inline void End() const { glBindVertexArray(0); }

		inline void DrawElements() const { glDrawElements(GL_TRIANGLES, GetNumQuadIndices(), GL_UNSIGNED_INT, 0); }

	private:

		FullscreenQuad()
		{
			size_t num_vertices = GetNumQuadVertices();
			size_t num_indices = GetNumQuadIndices();

			auto vertex_size = sizeof(GetQuadVertices()[0]);
			auto index_size = sizeof(GetQuadIndices()[0]);
			auto vertices_size = num_vertices * vertex_size;
			auto indices_size = num_indices * index_size;

			glGenBuffers(1, &vbo_);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_);
			glBufferData(GL_ARRAY_BUFFER, vertices_size, GetQuadVertices(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &ebo_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, GetQuadIndices(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			GLint attrib_index = 0;
			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);

			// Index data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

			// Vertex data
			glBindBuffer(GL_ARRAY_BUFFER, vbo_);

			// Position attribute
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (GLsizei)vertex_size, 0);
			glVertexAttribDivisor(0, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	};
}
