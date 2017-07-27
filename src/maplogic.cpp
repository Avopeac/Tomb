#include "maplogic.h"

#include "glm/gtx/component_wise.hpp"

using namespace game;

const float MapLogic::sqrt3_ = glm::sqrt(3.0f);

int32_t MapLogic::GetCubeDistance(HexCubeCoordinate a, HexCubeCoordinate b) const
{
	glm::ivec3 v0{ a.x, a.y, a.z };
	glm::ivec3 v1{ b.x, b.y, b.z };
	return glm::compMax(glm::abs(v0 - v1));
}

int32_t MapLogic::GetAxialDistance(HexCoordinate a, HexCoordinate b) const
{
	HexCubeCoordinate c0 = AxialToCube(a);
	HexCubeCoordinate c1 = AxialToCube(b);
	return GetCubeDistance(c0, c1);
}

int32_t MapLogic::GetOddRowOffsetDistance(HexCoordinate a, HexCoordinate b) const
{
	HexCubeCoordinate c0 = OddRowAxialToCube(a);
	HexCubeCoordinate c1 = OddRowAxialToCube(b);
	return GetCubeDistance(c0, c1);
}

int32_t MapLogic::GetEvenRowOffsetDistance(HexCoordinate a, HexCoordinate b) const
{
	HexCubeCoordinate c0 = EvenRowAxialToCube(a);
	HexCubeCoordinate c1 = EvenRowAxialToCube(b);
	return GetCubeDistance(c0, c1);
}

int32_t MapLogic::GetOddColumnOffsetDistance(HexCoordinate a, HexCoordinate b) const
{
	HexCubeCoordinate c0 = OddColumnAxialToCube(a);
	HexCubeCoordinate c1 = OddColumnAxialToCube(b);
	return GetCubeDistance(c0, c1);
}

int32_t MapLogic::GetEvenColumnOffsetDistance(HexCoordinate a, HexCoordinate b) const
{
	HexCubeCoordinate c0 = EvenColumnAxialToCube(a);
	HexCubeCoordinate c1 = EvenColumnAxialToCube(b);
	return GetCubeDistance(c0, c1);
}

HexCubeCoordinate MapLogic::GetCubeDiagonalNeighbor(HexCubeCoordinate cube, HexDirection direction) const
{
	HexCubeCoordinate neighbor = cube_diagonals_[static_cast<int>(direction)];
	neighbor.x += cube.x;
	neighbor.y += cube.y;
	neighbor.z += cube.z;
	return neighbor;
}

HexCoordinate MapLogic::GetAxialDirection(HexDirection direction) const
{
	return axial_directions_[static_cast<int>(direction)];
}

HexCoordinate MapLogic::GetAxialNeighbor(HexCoordinate axial, HexDirection direction) const
{
	HexCoordinate neighbor = GetAxialDirection(direction);
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

HexCubeCoordinate MapLogic::GetCubeDirection(HexDirection direction) const
{
	return cube_directions_[static_cast<int>(direction)];
}

HexCubeCoordinate MapLogic::GetCubeNeighbor(HexCubeCoordinate cube, HexDirection direction) const
{
	HexCubeCoordinate neighbor = GetCubeDirection(direction);
	neighbor.x += cube.x;
	neighbor.y += cube.y;
	neighbor.z += cube.z;
	return neighbor;
}

HexCoordinate MapLogic::CubeToAxial(HexCubeCoordinate cube) const
{
	HexCoordinate axial;
	axial.q = cube.x;
	axial.r = cube.z;
	return axial;
}

HexCubeCoordinate MapLogic::AxialToCube(HexCoordinate axial) const
{
	HexCubeCoordinate cube;
	cube.x = axial.q;
	cube.z = axial.r;
	cube.y = -cube.x - cube.z;
	return cube;
}

glm::vec2 MapLogic::AxialToCartesian(HexCoordinate axial, float size) const
{
	return { 2.0f * (axial.q + axial.r * 0.5f), 1.5f * axial.r };
}

glm::vec2 MapLogic::OddRowAxialToCartesian(HexCoordinate axial, float size) const
{
	return { sqrt3_ * (axial.q + 0.5 * (axial.r & 1)), 1.5f * axial.r };
}

glm::vec2 MapLogic::OddColumnAxialToCartesian(HexCoordinate axial, float size) const
{
	return { size * 1.5f * axial.q, size * sqrt3_ * (axial.r + 0.5 * (axial.q & 1)) };
}

glm::vec2 MapLogic::EvenRowAxialToCartesian(HexCoordinate axial, float size) const
{
	return { size * sqrt3_ * (axial.q - 0.5 * (axial.r & 1)), size * 1.5f * axial.r };
}

glm::vec2 MapLogic::EvenColumnAxialToCartesian(HexCoordinate axial, float size) const
{
	return { size * 1.5f * axial.q, size * sqrt3_ * (axial.r - 0.5 * (axial.q & 1)) };
}

HexCoordinate MapLogic::CubeToOddRowAxial(HexCubeCoordinate cube) const
{
	HexCoordinate axial;
	axial.q = cube.x + (cube.z - (cube.z & 1)) / 2;
	axial.r = cube.z;
	return axial;
}

HexCoordinate MapLogic::GetOddRowOffsetNeighbor(HexCoordinate axial, HexDirection direction) const
{
	int parity = axial.r & 1;
	int index = static_cast<int>(direction);
	HexCoordinate neighbor = hex_odd_row_directions_[parity][index];
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

HexCubeCoordinate MapLogic::OddRowAxialToCube(HexCoordinate axial) const
{
	HexCubeCoordinate cube;
	cube.x = axial.q - (axial.r - (axial.r & 1)) / 2;
	cube.z = axial.r;
	cube.y = -cube.x - cube.z;
	return cube;
}

HexCoordinate MapLogic::CubeToEvenRowAxial(HexCubeCoordinate cube) const
{
	HexCoordinate axial;
	axial.q = cube.x + (cube.z + (cube.z & 1)) / 2;
	axial.r = cube.z;
	return axial;
}

HexCoordinate MapLogic::GetEvenRowOffsetNeighbor(HexCoordinate axial, HexDirection direction) const
{
	int parity = axial.r & 1;
	int index = static_cast<int>(direction);
	HexCoordinate neighbor = hex_even_row_directions_[parity][index];
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

HexCubeCoordinate MapLogic::EvenRowAxialToCube(HexCoordinate axial) const
{
	HexCubeCoordinate cube;
	cube.x = axial.q - (axial.r + (axial.r & 1)) / 2;
	cube.z = axial.r;
	cube.y = -cube.x - cube.z;
	return cube;
}

HexCoordinate MapLogic::CubeToOddColumnAxial(HexCubeCoordinate cube) const
{
	HexCoordinate axial;
	axial.q = cube.x;
	axial.r = cube.z + (cube.x - (cube.x & 1)) / 2;
	return axial;
}

HexCoordinate MapLogic::GetOddColumnOffsetNeighbor(HexCoordinate axial, HexDirection direction) const
{
	int parity = axial.q & 1;
	int index = static_cast<int>(direction);
	HexCoordinate neighbor = hex_odd_column_directions_[parity][index];
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

HexCubeCoordinate MapLogic::OddColumnAxialToCube(HexCoordinate axial) const
{
	HexCubeCoordinate cube;
	cube.x = axial.q;
	cube.z = axial.r - (axial.q - (axial.q & 1)) / 2;
	cube.y = -cube.x - cube.z;
	return cube;
}

HexCoordinate MapLogic::CubeToEvenColumnAxial(HexCubeCoordinate cube) const
{
	HexCoordinate axial;
	axial.q = cube.x;
	axial.r = cube.z + (cube.x + (cube.x & 1)) / 2;
	return axial;
}

HexCoordinate MapLogic::GetEvenColumnOffsetNeighbor(HexCoordinate axial, HexDirection direction) const
{
	int parity = axial.q & 1;
	int index = static_cast<int>(direction);
	HexCoordinate neighbor = hex_even_column_directions_[parity][index];
	neighbor.r += axial.r;
	neighbor.q += axial.q;
	return neighbor;
}

HexCubeCoordinate MapLogic::EvenColumnAxialToCube(HexCoordinate axial) const
{
	HexCubeCoordinate cube;
	cube.x = axial.q;
	cube.z = axial.r - (axial.q + (axial.q & 1)) / 2;
	cube.y = -cube.x - cube.z;
	return cube;
}
