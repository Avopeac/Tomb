#pragma once

#include "SDL_assert.h"

#include "entity_types.h"

#include "component.h"

#include "system.h"

namespace entity {

	class EntityManager
	{

		EntityArray entities_;

		EntityId entity_unique_id_counter_;

		std::queue<EntityId> entity_unique_id_counter_queue_;
		 
		std::unordered_map<EntityId, std::string> entity_name_map_;

		EntityComponentArrays entity_components_;

		size_t systems_counter_;

		ComponentSystemsArray component_systems_;

	public:

		static EntityManager &Get()
		{
			static EntityManager instance;
			return instance;
		}

		~EntityManager();

		void Update(float delta_time);

		/// ENTITY

		const Entity * const CreateEntity(const std::string &name);

		void DeleteEntity(EntityId id);

		const Entity * const GetEntityByName(const std::string &name) const;

		const Entity * const GetEntityById(EntityId id) const;

		const std::string * const GetEntityNameById(EntityId id) const;

		/// COMPONENTS

		template <typename T, typename... Args> T * AddEntityComponent(EntityId id, Args&&... args);

		template <typename T> void RemoveEntityComponent(EntityId id);

		template <typename T> bool EntityHasComponent(EntityId id);

		template <typename T> T * GetEntityComponent(EntityId id);

		/// SYSTEMS

		void AddSystem(AbstractSystem * system);

	private:

		EntityManager();
	};

	template <typename T, typename... Args> T * EntityManager::AddEntityComponent(EntityId id, Args&&... args)
	{
		static_assert(std::is_base_of<Component, T>::value, "Derived class must be of base type Component.");
		RemoveEntityComponent<T>(id);
		
		if (id < MAX_ENTITIES && entities_[id]) 
		{
			EntityComponentKey key{ Component::GetId<T>() };
			T * component = new T(std::forward<Args>(args)...);
			entities_[id]->component_key |= key;
			entity_components_[id][key.to_ullong() - 1] = component;

			for (auto * system : component_systems_)
			{
				if (system)
				{
					system->TryInitialize(entities_[id]);
				}
			}

			return component;
		}

		return nullptr;
	}

	template <typename T> void EntityManager::RemoveEntityComponent(EntityId id)
	{
		static_assert(std::is_base_of<Component, T>::value, "Derived class must be of base type Component.");
		if (EntityHasComponent<T>(id))
		{
			EntityComponentKey key{ Component::GetId<T>() };

			for (auto * system : component_systems_)
			{
				if (system)
				{
					system->TryClean(entities_[id]);
				}
			}

			delete entity_components_[id][key.to_ullong() - 1];
			entity_components_[id][key.to_ullong() - 1] = nullptr;
			entities_[id]->component_key &= key.flip();
		}
	}

	template <typename T> bool EntityManager::EntityHasComponent(EntityId id)
	{
		static_assert(std::is_base_of<Component, T>::value, "Derived class must be of base type Component.");
		EntityComponentKey key{ Component::GetId<T>() };
		return id < MAX_ENTITIES && entities_[id] ? (key & entities_[id]->component_key) == key : false;
	}

	template <typename T> T * EntityManager::GetEntityComponent(EntityId id)
	{
		static_assert(std::is_base_of<Component, T>::value, "Derived class must be of base type Component.");
		EntityComponentKey key{ Component::GetId<T>() };
		return id < MAX_ENTITIES && entities_[id] ? static_cast<T*>(entity_components_[id][key.to_ullong() - 1]) : nullptr;
	}
}
