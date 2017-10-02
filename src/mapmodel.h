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

	class MapModel
	{
		const MapLogic &logic_;

		uint32_t map_size_;

		std::unordered_map<HexCoordinate, MapTileType, HexCoordinateHash> tiles_;

	public:

		MapModel(uint32_t map_size, MapLogic logic) :
			map_size_(map_size),
			logic_(logic)
		{
			Build();
		}

		~MapModel() {}

		inline auto GetTileBeginIterator() const { return tiles_.cbegin(); }

		inline auto GetTileEndIterator() const { return tiles_.cend(); }

	private:

		void Build();

		void BuildFlatHexagonMap();
	};
}