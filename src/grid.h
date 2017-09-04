#pragma once

#include "mesh.h"

namespace graphics
{
	class Grid
	{

	public:

		static void GetMeshData(size_t resolution, std::vector<MeshVertex> &vertices, std::vector<Uint32> &indices);
	};
}