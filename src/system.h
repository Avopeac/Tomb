#pragma once

#include <bitset>

#include "abstract_system.h"

#include "entity.h"

namespace entity 
{
	template <typename T, typename... Components> class System : public AbstractSystem
	{
		template <typename S, typename... Cs> std::bitset<max_defined_components> GetRequiredComponentKey() const
		{
			const std::initializer_list<size_t> component_ids{ Component::GetTypeId<Cs>()... };

			std::bitset<max_defined_components> required_component_key = 0;

			for (auto &id : component_ids)
			{
				required_component_key |= std::bitset<max_defined_components>{ id };
			}

			return required_component_key;
		}

		std::bitset<max_defined_components> required_component_keys_ = GetRequiredComponentKey<T, Components...>();

	public:

		virtual ~System() {}

		virtual void TryInitialize(Entity * entity) override
		{
			if ((entity->GetComponentKey() & required_component_keys_) == required_component_keys_)
			{
				static_cast<T*>(this)->Initialize(entity);
			}
		}

		virtual void TryClean(Entity * entity) override
		{
			if ((entity->GetComponentKey() & required_component_keys_) == required_component_keys_)
			{
				static_cast<T*>(this)->Clean(entity);
			}
		}

		virtual void TryUpdate(Entity * entity) override
		{
			if ((entity->GetComponentKey() & required_component_keys_) == required_component_keys_)
			{
				static_cast<T*>(this)->Update(entity);
			}
		}

	protected:

		System() {}

	};
}
