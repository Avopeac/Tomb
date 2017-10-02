#include "mapmodel.h"

using namespace game;

void MapModel::Build()
{
	tiles_.clear();

	BuildFlatHexagonMap();
}

void MapModel::BuildFlatHexagonMap()
{
	HexCubeCoordinate cube;

	int half_size = (int)map_size_ / 2;

	for (int i = -half_size; i <= half_size; ++i)
	{
		for (int j = -half_size; j <= half_size; ++j)
		{
			for (int k = -half_size; k <= half_size; ++k)
			{
				cube.x = k;
				cube.y = j;
				cube.z = i;

				if (cube.x + cube.y + cube.z == 0)
				{
					tiles_[logic_.CubeToAxial(cube)] = MapTileType::Sand;
				}
			}
		}
	}
}
