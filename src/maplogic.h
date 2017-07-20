#pragma once

#include "config.h"

namespace game
{

#define NUM_HEX_DIRECTIONS 6

	struct HexCubeCoordinate
	{
		int32_t x, y, z;

		bool operator==(const HexCubeCoordinate &other)
		{
			return x == other.x && y == other.y && z == other.z;
		}
	};

	struct HexCoordinate
	{
		int32_t q, r;
	};

	inline bool operator==(const HexCoordinate& lhs, const HexCoordinate& rhs)
	{
		return lhs.q == rhs.q && lhs.r == rhs.r;
	}

	struct HexCoordinateHash
	{
		std::size_t operator()(HexCoordinate const & coordinate) const
		{
			std::size_t h1 = std::hash<int32_t>{}(coordinate.q);
			std::size_t h2 = std::hash<int32_t>{}(coordinate.r);
			return h1 ^ (h2 << 1);
		}
	};

	enum class HexDirection
	{
		Right = 0,
		UpRight,
		UpLeft,
		Left,
		DownLeft,
		DownRight,
	};

	/// Credit goes to Amit at http://www.redblobgames.com

	class MapLogic
	{

		HexCoordinate axial_directions_[NUM_HEX_DIRECTIONS] = { { 1,0 },{ 1,-1 },{ 0,-1 },{ -1,0 },{ -1,1 },{ 0,1 } };

		HexCubeCoordinate cube_directions_[NUM_HEX_DIRECTIONS] = { { 1,-1,0 },{ 1,0,-1 },{ 0,1,-1 },{ -1,1,0 },{ -1,0,1 },{ 0,-1,1 } };

		HexCubeCoordinate cube_diagonals_[NUM_HEX_DIRECTIONS] = { { 2,-1,-1 },{ 1,1,-2 },{ -1,2,-1 },{ -2,1,1 },{ -1,-1,2 },{ 1,-2,1 } };

		HexCoordinate hex_odd_row_directions_[2][NUM_HEX_DIRECTIONS] = { 
			{ { 1,0 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ -1,1 },{ 0,1 } },
			{ { 1,0 },{ 1,-1 },{ 0,-1 },{ -1,0 },{ 0,1 },{ 1,1 } }
		};

		HexCoordinate hex_even_row_directions_[2][NUM_HEX_DIRECTIONS] = { 
			{ { 1,0 },{ 1,-1 },{ 0,-1 },{ -1,0 },{ 0,1 },{ 1,1 } },
		    { { 1,0 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ -1,1 },{ 0,1 } }
		};

		HexCoordinate hex_odd_column_directions_[2][NUM_HEX_DIRECTIONS] = { 
			{ { 1,0 },{ 1,-1 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ 0,1 } },
			{ { 1,1 },{ 1,0 },{ 0,-1 },{ -1,0 },{ -1,1 },{ 0,1 } }
		};

		HexCoordinate hex_even_column_directions_[2][NUM_HEX_DIRECTIONS] = { 
			{ { 1,1 },{ 1,0 },{ 0,-1 },{ -1,0 },{ -1,1 },{ 0,1 } },
			{ { 1,0 },{ 1,-1 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ 0,1 } }
		};

	public:

		MapLogic() {}

		~MapLogic() {};

		/// ALGEBRA
		
		inline int32_t GetCubeDistance(HexCubeCoordinate a, HexCubeCoordinate b);

		inline int32_t GetAxialDistance(HexCoordinate a, HexCoordinate b);

		inline int32_t GetOddRowOffsetDistance(HexCoordinate a, HexCoordinate b);

		inline int32_t GetEvenRowOffsetDistance(HexCoordinate a, HexCoordinate b);

		inline int32_t GetOddColumnOffsetDistance(HexCoordinate a, HexCoordinate b);

		inline int32_t GetEvenColumnOffsetDistance(HexCoordinate a, HexCoordinate b);

		/// NEIGHBORHOOD

		// Nearest neighbor fetch diagonally
		inline HexCubeCoordinate GetCubeDiagonalNeighbor(HexCubeCoordinate cube, HexDirection direction);

		// Directional offset for axial coordinates
		inline HexCoordinate GetAxialDirection(HexDirection direction);

		// Neigbor fetch for axial coordinates
		inline HexCoordinate GetAxialNeighbor(HexCoordinate axial, HexDirection direction);

		// Directional offset for cube coordinates
		inline HexCubeCoordinate GetCubeDirection(HexDirection direction);

		// Neigbor fetch for cube coordinates
		inline HexCubeCoordinate GetCubeNeighbor(HexCubeCoordinate cube, HexDirection direction);

		/// COORDINATE CONVERSION

		// Conversion between cube to axial coordinates
		inline HexCoordinate CubeToAxial(HexCubeCoordinate cube);

		// Conversion between axial to cube coordinates
		inline HexCubeCoordinate AxialToCube(HexCoordinate axial);

		/// PLACEMENT

		// 1. Shoves odd rows by +½ column
		inline HexCoordinate CubeToOddRowAxial(HexCubeCoordinate cube);

		inline HexCoordinate GetOddRowOffsetNeighbor(HexCoordinate axial, HexDirection direction);

		// Conversion back from 1.
		inline HexCubeCoordinate OddRowAxialToCube(HexCoordinate axial);
		
		// 2. Shoves even rows by +½ column
		inline HexCoordinate CubeToEvenRowAxial(HexCubeCoordinate cube);

		inline HexCoordinate GetEvenRowOffsetNeighbor(HexCoordinate axial, HexDirection direction);

		// Conversion back from 2.
		inline HexCubeCoordinate EvenRowAxialToCube(HexCoordinate axial);
		
		// 3. Shoves odd columns by +½ row
		inline HexCoordinate CubeToOddColumnAxial(HexCubeCoordinate cube);

		inline HexCoordinate GetOddColumnOffsetNeighbor(HexCoordinate axial, HexDirection direction);

		// Conversion back from 3.
		inline HexCubeCoordinate OddColumnAxialToCube(HexCoordinate axial);

		// 4. Shoves even columns by +½ row
		inline HexCoordinate CubeToEvenColumnAxial(HexCubeCoordinate cube);
		
		inline HexCoordinate GetEvenColumnOffsetNeighbor(HexCoordinate axial, HexDirection direction);

		// Conversion back from 4.
		inline HexCubeCoordinate EvenColumnAxialToCube(HexCoordinate axial);
	};
}
