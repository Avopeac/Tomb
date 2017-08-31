#pragma once

#pragma once

#include <unordered_map>

#include "GL/glew.h"

#include "SDL.h"

#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"

#include "disposable.h"

namespace graphics
{
	struct MeshVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;

		bool operator==(const MeshVertex &other) const
		{
			return position == other.position &&
				normal == other.normal &&
				texcoord == other.texcoord;
		}
	};

	struct Mesh
	{
		GLuint vao;
		GLuint vbo;
		GLuint ebo;
		Uint32 num_meshes;
		std::vector<Uint32> indices;
		std::vector<Uint32> num_indices;
	};

	class MeshCache
	{
		std::unordered_map<size_t, Mesh> meshes_;

	public:

		MeshCache();

		~MeshCache();

		Mesh &GetFromFile(size_t &hash, const std::string &name, const std::string &file_path);

		Mesh &GetFromHash(size_t hash);
	};
}

namespace std
{
	template<> struct hash<graphics::MeshVertex>
	{
		size_t operator()(graphics::MeshVertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texcoord) << 1);
		}
	};
}
