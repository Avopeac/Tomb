#pragma once

#include "glm/glm.hpp"

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
		static const float sqrt3_;

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

		int32_t GetCubeDistance(HexCubeCoordinate a, HexCubeCoordinate b) const;

		int32_t GetAxialDistance(HexCoordinate a, HexCoordinate b) const;

		int32_t GetOddRowOffsetDistance(HexCoordinate a, HexCoordinate b) const;

		int32_t GetEvenRowOffsetDistance(HexCoordinate a, HexCoordinate b) const;

		int32_t GetOddColumnOffsetDistance(HexCoordinate a, HexCoordinate b) const;

		int32_t GetEvenColumnOffsetDistance(HexCoordinate a, HexCoordinate b) const;

		/// NEIGHBORHOOD

		// Nearest neighbor fetch diagonally
		HexCubeCoordinate GetCubeDiagonalNeighbor(HexCubeCoordinate cube, HexDirection direction) const;

		// Directional offset for axial coordinates
		HexCoordinate GetAxialDirection(HexDirection direction) const;

		// Neigbor fetch for axial coordinates
		HexCoordinate GetAxialNeighbor(HexCoordinate axial, HexDirection direction) const;

		// Directional offset for cube coordinates
		HexCubeCoordinate GetCubeDirection(HexDirection direction) const;

		// Neigbor fetch for cube coordinates
		HexCubeCoordinate GetCubeNeighbor(HexCubeCoordinate cube, HexDirection direction) const;

		/// COORDINATE CONVERSION

		// Conversion between cube to axial coordinates
		HexCoordinate CubeToAxial(HexCubeCoordinate cube) const;

		// Conversion between axial to cube coordinates
		HexCubeCoordinate AxialToCube(HexCoordinate axial) const;

		glm::vec2 AxialToCartesian(HexCoordinate axial, float size) const;
		
		glm::vec2 OddRowAxialToCartesian(HexCoordinate axial, float size) const;

		glm::vec2 OddColumnAxialToCartesian(HexCoordinate axial, float size) const;

		glm::vec2 EvenRowAxialToCartesian(HexCoordinate axial, float size) const;

		glm::vec2 EvenColumnAxialToCartesian(HexCoordinate axial, float size) const;

		HexCoordinate CartesianToAxial(glm::vec2, float size) const;

		/// PLACEMENT

		// 1. Shoves odd rows by +½ column
		HexCoordinate CubeToOddRowAxial(HexCubeCoordinate cube) const;

		HexCoordinate GetOddRowOffsetNeighbor(HexCoordinate axial, HexDirection direction) const;

		// Conversion back from 1.
		HexCubeCoordinate OddRowAxialToCube(HexCoordinate axial) const;

		// 2. Shoves even rows by +½ column
		HexCoordinate CubeToEvenRowAxial(HexCubeCoordinate cube) const;

		HexCoordinate GetEvenRowOffsetNeighbor(HexCoordinate axial, HexDirection direction) const;

		// Conversion back from 2.
		HexCubeCoordinate EvenRowAxialToCube(HexCoordinate axial) const;

		// 3. Shoves odd columns by +½ row
		HexCoordinate CubeToOddColumnAxial(HexCubeCoordinate cube) const;

		HexCoordinate GetOddColumnOffsetNeighbor(HexCoordinate axial, HexDirection direction) const;

		// Conversion back from 3.
		HexCubeCoordinate OddColumnAxialToCube(HexCoordinate axial) const;

		// 4. Shoves even columns by +½ row
		HexCoordinate CubeToEvenColumnAxial(HexCubeCoordinate cube) const;

		HexCoordinate GetEvenColumnOffsetNeighbor(HexCoordinate axial, HexDirection direction) const;

		// Conversion back from 4.
		HexCubeCoordinate EvenColumnAxialToCube(HexCoordinate axial) const;
	};
}
