#include "maplogic.h"

#include "glm/glm.hpp"
#include "glm/gtx/component_wise.hpp"

using namespace game;

inline int32_t MapLogic::GetCubeDistance(HexCubeCoordinate a, HexCubeCoordinate b)
{
	glm::ivec3 v0{ a.x, a.y, a.z };
	glm::ivec3 v1{ b.x, b.y, b.z };
	return glm::compMax(glm::abs(v0 - v1));
}

inline int32_t MapLogic::GetAxialDistance(HexCoordinate a, HexCoordinate b)
{
	HexCubeCoordinate c0 = AxialToCube(a);
	HexCubeCoordinate c1 = AxialToCube(b);
	return GetCubeDistance(c0, c1);
}

inline int32_t MapLogic::GetOddRowOffsetDistance(HexCoordinate a, HexCoordinate b)
{
	HexCubeCoordinate c0 = OddRowAxialToCube(a);
	HexCubeCoordinate c1 = OddRowAxialToCube(b);
	return GetCubeDistance(c0, c1);
}

inline int32_t MapLogic::GetEvenRowOffsetDistance(HexCoordinate a, HexCoordinate b)
{
	HexCubeCoordinate c0 = EvenRowAxialToCube(a);
	HexCubeCoordinate c1 = EvenRowAxialToCube(b);
	return GetCubeDistance(c0, c1);
}

inline int32_t MapLogic::GetOddColumnOffsetDistance(HexCoordinate a, HexCoordinate b)
{
	HexCubeCoordinate c0 = OddColumnAxialToCube(a);
	HexCubeCoordinate c1 = OddColumnAxialToCube(b);
	return GetCubeDistance(c0, c1);
}

inline int32_t MapLogic::GetEvenColumnOffsetDistance(HexCoordinate a, HexCoordinate b)
{
	HexCubeCoordinate c0 = EvenColumnAxialToCube(a);
	HexCubeCoordinate c1 = EvenColumnAxialToCube(b);
	return GetCubeDistance(c0, c1);
}

inline HexCubeCoordinate MapLogic::GetCubeDiagonalNeighbor(HexCubeCoordinate cube, HexDirection direction)
{
	HexCubeCoordinate neighbor = cube_diagonals_[static_cast<int>(direction)];
	neighbor.x += cube.x;
	neighbor.y += cube.y;
	neighbor.z += cube.z;
	return neighbor;
}

inline HexCoordinate MapLogic::GetAxialDirection(HexDirection direction)
{
	return axial_directions_[static_cast<int>(direction)];
}

inline HexCoordinate MapLogic::GetAxialNeighbor(HexCoordinate axial, HexDirection direction)
{
	HexCoordinate neighbor = GetAxialDirection(direction);
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

inline HexCubeCoordinate MapLogic::GetCubeDirection(HexDirection direction)
{
	return cube_directions_[static_cast<int>(direction)];
}

inline HexCubeCoordinate MapLogic::GetCubeNeighbor(HexCubeCoordinate cube, HexDirection direction)
{
	HexCubeCoordinate neighbor = GetCubeDirection(direction);
	neighbor.x += cube.x;
	neighbor.y += cube.y;
	neighbor.z += cube.z;
	return neighbor;
}

inline HexCoordinate MapLogic::CubeToAxial(HexCubeCoordinate cube)
{
	HexCoordinate axial;
	axial.q = cube.x;
	axial.r = cube.z;
	return axial;
}

inline HexCubeCoordinate MapLogic::AxialToCube(HexCoordinate axial)
{
	HexCubeCoordinate cube;
	cube.x = axial.q;
	cube.z = axial.r;
	cube.y = -cube.x - cube.z;
	return cube;
}

inline HexCoordinate MapLogic::CubeToOddRowAxial(HexCubeCoordinate cube)
{
	HexCoordinate axial;
	axial.q = cube.x + (cube.z - (cube.z & 1)) / 2;
	axial.r = cube.z;
	return axial;
}

inline HexCoordinate MapLogic::GetOddRowOffsetNeighbor(HexCoordinate axial, HexDirection direction)
{
	int parity = axial.r & 1;
	int index = static_cast<int>(direction);
	HexCoordinate neighbor = hex_odd_row_directions_[parity][index];
	neighbor.r += axial.r; 
	neighbor.q += axial.q;
	return neighbor;
}

inline HexCubeCoordinate MapLogic::OddRowAxialToCube(HexCoordinate axial)
{
	HexCubeCoordinate cube;
	cube.x = axial.q - (axial.r - (axial.r & 1)) / 2;
	cube.z = axial.r;
	cube.y = -cube.x - cube.z;
	return cube;
}

inline HexCoordinate MapLogic::CubeToEvenRowAxial(HexCubeCoordinate cube)
{
	HexCoordinate axial;
	axial.q = cube.x + (cube.z + (cube.z & 1)) / 2;
	axial.r = cube.z;
	return axial;
}

inline HexCoordinate MapLogic::GetEvenRowOffsetNeighbor(HexCoordinate axial, HexDirection direction)
{
	int parity = axial.r & 1;
	int index = static_cast<int>(direction);
	HexCoordinate neighbor = hex_even_row_directions_[parity][index];
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

inline HexCubeCoordinate MapLogic::EvenRowAxialToCube(HexCoordinate axial)
{
	HexCubeCoordinate cube;
	cube.x = axial.q - (axial.r + (axial.r & 1)) / 2;
	cube.z = axial.r;
	cube.y = -cube.x - cube.z;
	return cube;
}

inline HexCoordinate MapLogic::CubeToOddColumnAxial(HexCubeCoordinate cube)
{
	HexCoordinate axial;
	axial.q = cube.x;
	axial.r = cube.z + (cube.x - (cube.x & 1)) / 2;
	return axial;
}

inline HexCoordinate MapLogic::GetOddColumnOffsetNeighbor(HexCoordinate axial, HexDirection direction)
{
	int parity = axial.q & 1;
	int index = static_cast<int>(direction);
	HexCoordinate neighbor = hex_odd_column_directions_[parity][index];
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

inline HexCubeCoordinate MapLogic::OddColumnAxialToCube(HexCoordinate axial)
{
	HexCubeCoordinate cube;
	cube.x = axial.q;
	cube.z = axial.r - (axial.q - (axial.q & 1)) / 2;
	cube.y = -cube.x - cube.z;
	return cube;
}

inline HexCoordinate MapLogic::CubeToEvenColumnAxial(HexCubeCoordinate cube)
{
	HexCoordinate axial;
	axial.q = cube.x;
	axial.r = cube.z + (cube.x + (cube.x & 1)) / 2;
	return axial;
}

inline HexCoordinate MapLogic::GetEvenColumnOffsetNeighbor(HexCoordinate axial, HexDirection direction)
{
	int parity = axial.q & 1;
	int index = static_cast<int>(direction);
	HexCoordinate neighbor = hex_even_column_directions_[parity][index];
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

inline HexCubeCoordinate MapLogic::EvenColumnAxialToCube(HexCoordinate axial)
{
	HexCubeCoordinate cube;
	cube.x = axial.q;
	cube.z = axial.r - (axial.q + (axial.q & 1)) / 2;
	cube.y = -cube.x - cube.z;
	return cube;
}
