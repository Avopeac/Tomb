#include "mapmodel.h"

using namespace game;

void MapModel::Build()
{
	tiles_.clear();

	switch (shape_type_)
	{
		case MapShapeType::Triangle: { BuildTriangleMap(); } break;
		case MapShapeType::Rectangle: { BuildRectangleMap(); } break;
		case MapShapeType::Rhombus: { BuildRhombusMap(); } break;
		case MapShapeType::FlatHexagon: { BuildFlatHexagonMap(); } break;
		case MapShapeType::SharpHexagon: { BuildSharpHexagonMap(); } break;
	}
}

void MapModel::BuildTriangleMap()
{
}

void MapModel::BuildRectangleMap()
{ 
}

void MapModel::BuildRhombusMap()
{
}

void MapModel::BuildSharpHexagonMap()
{
}

void MapModel::BuildFlatHexagonMap()
{
	HexCubeCoordinate cube;

	for (int i = -3; i <= 3; ++i)
	{
		for (int j = -3; j <= 3; ++j)
		{
			for (int k = -3; k <= 3; ++k)
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
