#pragma once

#include <string>
#include <array>
#include <memory>
#include <bitset>
#include <vector>
#include <queue>
#include <unordered_map>

namespace entity 
{

	struct Entity;

	class Component;

	class AbstractSystem;

	constexpr size_t MAX_ENTITIES = 1 << 16;

	constexpr size_t MAX_COMPONENTS = 1 << 8;

	constexpr size_t MAX_SYSTEMS = 1 << 8;

	using EntityId = size_t;

	using EntityArray = std::array<Entity *, MAX_ENTITIES>;

	using EntityComponentArray = std::array<Component *, MAX_COMPONENTS>;

	using EntityComponentArrays = std::array<EntityComponentArray, MAX_ENTITIES>;

	using EntityComponentKey = std::bitset<MAX_COMPONENTS>;

	using ComponentSystemsArray = std::array<AbstractSystem *, MAX_SYSTEMS>;

	struct Entity
	{
		EntityId id;
		EntityComponentKey component_key;
		Entity(EntityId id) : id(id), component_key(0) {}
	};
}
