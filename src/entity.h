#pragma once

#include <array>
#include <memory>
#include <bitset>
#include <vector>

#include "SDL_assert.h"

#include "component.h"

namespace entity {

	class Entity
	{
		size_t uid_;

		std::bitset<max_defined_components> component_key_;

		std::array<std::unique_ptr<Component>, max_defined_components> components_{ nullptr };

	public:

		Entity(size_t uid) : uid_(uid) {}

		~Entity() {}

		Entity(const Entity &other) = delete;

		Entity(Entity &&other) = delete;

		Entity &operator=(const Entity &other) = delete;

		Entity &operator=(Entity &&other) = delete;

		template <typename T, typename... Args> T * AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "Derived class must be of base type Component.");
			
			RemoveComponent<T>();

			auto index = Component::GetTypeId<T>() - 1;
			component_key_.set(index);
			components_[index] = std::move(std::make_unique<T>(this, std::forward<Args>(args)...));
			return static_cast<T*>(components_[index].get());
		}

		template <typename T> void RemoveComponent()
		{
			if (HasComponent<T>())
			{
				auto index = Component::GetTypeId<T>() - 1;
				const std::bitset<max_defined_components> component_key{ index };
				component_key_ &= component_key;
				components_[index].reset(nullptr);
			}
		}

		template <typename T> bool HasComponent()
		{
			static_assert(std::is_base_of<Component, T>::value, "Derived class must be of base type Component.");
			const std::bitset<max_defined_components> component_key{ Component::GetTypeId<T>() };
			return (component_key & component_key_) == component_key;
		}

		template <typename T> T * GetComponent()
		{
			const std::bitset<max_defined_components> component_key{ Component::GetTypeId<T>() };
			return static_cast<T*>(components_[component_key.to_ullong() - 1].get());
		}

		inline size_t GetUid() const { return uid_; }

		inline const std::bitset<max_defined_components> &GetComponentKey() const { return component_key_; }
	};
}
