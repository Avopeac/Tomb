#pragma once

#include <unordered_map>
#include <functional>

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
		const MapLogic &logic_;

		uint32_t convex_hull_width_;

		uint32_t convex_hull_height_;

		MapShapeType shape_type_;

		std::unordered_map<HexCoordinate, MapTileType, HexCoordinateHash> tiles_;

	public:

		MapModel(uint32_t convex_hull_width, uint32_t convex_hull_height, MapShapeType shape_type, MapLogic logic) :
			convex_hull_width_(convex_hull_width),
			convex_hull_height_(convex_hull_height),
			shape_type_(shape_type),
			logic_(logic)
		{
			Build();
		}

		~MapModel() {}

		inline auto GetTileBeginIterator() const { return tiles_.cbegin(); }
		inline auto GetTileEndIterator() const { return tiles_.cend(); }

	private:

		void Build();

		void BuildTriangleMap();

		void BuildRectangleMap();

		void BuildRhombusMap();

		void BuildSharpHexagonMap();

		void BuildFlatHexagonMap();
	};
}