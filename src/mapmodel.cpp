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

	int height = convex_hull_height_;
	int half_width = convex_hull_width_ / 2;

	HexCoordinate hex;

	int narrowing = 0;
	for (int r = 0; r < height; ++r)
	{
		hex.r = r;

		for (int q = -half_width + narrowing; q <= half_width - narrowing; ++q)
		{

			hex.q = q;
		
			tiles_[hex] = MapTileType::Sand;
		}
	}
}

void MapModel::BuildRectangleMap()
{ 
	int half_height = convex_hull_height_ / 2;
	int half_width = convex_hull_width_ / 2;

	HexCoordinate hex;

	for (int r = -half_height; r < half_height; ++r)
	{
		hex.r = r;

		for (int q = -half_width; q < half_width; ++q)
		{

			hex.q = q;

			tiles_[hex] = MapTileType::Sand;
		}
	}
}

void MapModel::BuildRhombusMap()
{
	int half_height = convex_hull_height_ / 2;
	int half_width = convex_hull_width_ / 2;

	HexCoordinate hex;

	int offset = 0;

	for (int r = -half_height; r < half_height; ++r)
	{
		hex.r = r;

		for (int q = -half_width + offset; q <= half_width + offset; ++q)
		{

			hex.q = q;

			tiles_[hex] = MapTileType::Sand;
		}

		++offset;
	}
}

void MapModel::BuildSharpHexagonMap()
{
}

void MapModel::BuildFlatHexagonMap()
{
	int start_z = -3;
	int end_z = 3;

	int row = 0;
	int x = 0;
	int y = 3;

	for (int z = start_z; z <= end_z; ++z)
	{
		for (int j = row; j <= 3; ++j)
		{

			HexCubeCoordinate cube;
			cube.x = x;
			cube.y = y;
			cube.z = z;

			HexCoordinate hex = logic_.CubeToAxial(cube);
			tiles_[hex] = MapTileType::Sand;

			x++;
			y--;
		}

		
		row--;

		x = row;
		y = z <= 0 ? 3 : 3 - z;

	}
}
