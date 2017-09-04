#include "grid.h"

using namespace graphics;

void Grid::GetMeshData(size_t resolution, std::vector<MeshVertex>& vertices, std::vector<Uint32>& indices)
{
	vertices.clear();
	vertices.resize((resolution + 1) * (resolution + 1));

	float step_size = 1.0f / resolution;
	for (size_t v = 0, y = 0; y <= resolution; y++)
	{
		for (size_t x = 0; x <= resolution; x++, v++)
		{
			vertices[v].position = glm::vec3(x * step_size - 0.5f, y * step_size - 0.5f, 0.0f);
			vertices[v].normal = glm::vec3(0, 0, 1);
			vertices[v].texcoord = glm::vec2(x, y) * step_size;
		}
	}

	indices.clear();
	indices.resize(resolution * resolution * 6); 

	for (size_t t = 0, v = 0, y = 0; y < resolution; y++, v++)
	{
		for (size_t x = 0; x < resolution; x++, v++, t += 6)
		{
			indices[t] = Uint32(v);
			indices[t + 1] = Uint32(v + 1);
			indices[t + 2] = Uint32(v + resolution + 1);
			indices[t + 3] = Uint32(v + resolution + 1);
			indices[t + 4] = Uint32(v + 1);
			indices[t + 5] = Uint32(v + resolution + 2);
		}
	}
}
