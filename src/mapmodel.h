#pragma once

#include <unordered_map>

#include "maplogic.h"

namespace game
{

	enum class MapTileType
	{
		Unexplored,
		Sand,
		Storm,
		Tomb,
		Artifact,
		Waterhole,
		Camp,
		Mercenaries,
	};

	enum class MapShapeType
	{
		Triangle,
		Rhombus,
		Rectangle,
		SharpHexagon,
		FlatHexagon,
	};

	class MapModel
	{

		uint32_t convex_hull_width_;

		uint32_t convex_hull_height_;

		MapShapeType shape_type_;

		std::unordered_map<HexCoordinate, MapTileType> tiles_;

	public:

		MapModel(uint32_t convex_hull_width, uint32_t convex_hull_height, MapShapeType shape_type) :
			convex_hull_width_(convex_hull_width),
			convex_hull_height_(convex_hull_height),
			shape_type_(shape_type)
		{
			Build();
		}

		~MapModel() {}

		inline auto GetTileIterator() const { return tiles_.cbegin(); }

	private:

		void Build();

		void BuildTriangleMap();

		void BuildRectangleMap();

		void BuildRhombusMap();

		void BuildSharpHexagonMap();

		void BuildFlatHexagonMap();
	};
}