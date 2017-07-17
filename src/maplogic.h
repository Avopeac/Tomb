#pragma once

#include "config.h"

namespace game
{
	struct CubeCoordinate
	{
		int32_t x, y, z;
	};

	struct AxialCoordinate
	{
		int32_t q, r;
	};

	enum class CubeDirection
	{
		Right = 0,
		UpRight,
		UpLeft,
		Left,
		DownLeft,
		DownRight,
	};

	enum class AxialDirection
	{
		Right = 0,
		UpRight,
		UpLeft,
		Left,
		DownLeft,
		DownRight,
	};

	class MapLogic
	{

		AxialCoordinate axial_directions_[] =
		{
			{ 1, 0 },
			{ 1, -1 },
			{ 0, -1 },
			{ -1, 0 },
			{ -1, 1 },
			{ 0, 1 }
		};

		CubeCoordinate cube_directions_[] =
		{
			{ 1, -1, 0 },
			{ 1, 0, -1 },
			{ 0, 1, -1 },
			{ -1, 1, 0 },
			{ -1, 0, 1 },
			{ 0, -1, 1 }
		};

	public:

		MapLogic();

		~MapLogic();

		inline AxialCoordinate GetAxialDirection(AxialDirection direction)
		{
			return axial_directions_[static_cast<int>(direction)];
		}

		inline AxialCoordinate GetCubeNeighbor(AxialCoordinate axial, AxialDirection direction)
		{
			AxialCoordinate neighbor = GetAxialDirection(direction);
			neighbor.r += axial.r;
			neighbor.q += axial.q;
			return neighbor;
		}

		inline CubeCoordinate GetCubeDirection(CubeDirection direction)
		{
			return cube_directions_[static_cast<int>(direction)];
		}

		inline CubeCoordinate GetCubeNeighbor(CubeCoordinate cube, CubeDirection direction)
		{
			CubeCoordinate neighbor = GetCubeDirection(direction);
			neighbor.x += cube.x;
			neighbor.y += cube.y;
			neighbor.z += cube.z;
			return neighbor;
		}

		// Conversion between cube to axial coordinates
		inline AxialCoordinate CubeToAxial(CubeCoordinate cube)
		{
			AxialCoordinate axial;
			axial.q = cube.x;
			axial.r = cube.z;
			return axial;
		}

		// Conversion between axial to cube coordinates
		inline CubeCoordinate AxialToCube(AxialCoordinate axial)
		{
			CubeCoordinate cube;
			cube.x = axial.q;
			cube.z = axial.r;
			cube.y = -cube.x - cube.z;
			return cube;
		}

		// 1. Shoves odd rows by +½ column
		inline AxialCoordinate CubeToOddRowAxial(CubeCoordinate cube)
		{
			AxialCoordinate axial;
			axial.q = cube.x + (cube.z - (cube.z & 1)) / 2;
			axial.r = cube.z;
			return axial;
		}

		// Conversion back from 1.
		inline CubeCoordinate OddRowAxialToCube(AxialCoordinate axial)
		{
			CubeCoordinate cube;
			cube.x = axial.q - (axial.r - (axial.r & 1)) / 2;
			cube.z = axial.r;
			cube.y = -cube.x - cube.z;
			return cube;
		}

		// 2. Shoves even rows by +½ column
		inline AxialCoordinate CubeToEvenRowAxial(CubeCoordinate cube)
		{
			AxialCoordinate axial;
			axial.q = cube.x + (cube.z + (cube.z & 1)) / 2;
			axial.r = cube.z;
			return axial;
		}

		// Conversion back from 2.
		inline CubeCoordinate EvenRowAxialToCube(AxialCoordinate axial)
		{
			CubeCoordinate cube;
			cube.x = axial.q - (axial.r + (axial.r & 1)) / 2;
			cube.z = axial.r;
			cube.y = -cube.x - cube.z;
			return cube;
		}

		// 3. Shoves odd columns by +½ row
		inline AxialCoordinate CubeToOddColumnAxial(CubeCoordinate cube)
		{
			AxialCoordinate axial;
			axial.q = cube.x;
			axial.r = cube.z + (cube.x - (cube.x & 1)) / 2;
			return axial;
		}

		// Conversion back from 3.
		inline CubeCoordinate OddColumnAxialToCube(AxialCoordinate axial)
		{
			CubeCoordinate cube;
			cube.x = axial.q;
			cube.z = axial.r - (axial.q - (axial.q & 1)) / 2;
			cube.y = -cube.x - cube.z;
			return cube;
		}

		// 4. Shoves even columns by +½ row
		inline AxialCoordinate CubeToEvenColumnAxial(CubeCoordinate cube)
		{
			AxialCoordinate axial;
			axial.q = cube.x;
			axial.r = cube.z + (cube.x + (cube.x & 1)) / 2;
			return axial;
		}

		// Conversion back from 4.
		inline CubeCoordinate EvenColumnAxialToCube(AxialCoordinate axial)
		{
			CubeCoordinate cube;
			cube.x = axial.q;
			cube.z = axial.r - (axial.q + (axial.q & 1)) / 2;
			cube.y = -cube.x - cube.z;
			return cube;
		}
	};
}
