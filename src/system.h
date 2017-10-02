#pragma once

#include "abstract_system.h"

namespace entity
{
	template <typename T, typename... Components> class System : public AbstractSystem
	{
		template <typename S, typename... Cs> EntityComponentKey GetRequiredComponentKey() const
		{
			const std::initializer_list<size_t> component_ids{ Component::GetId<Cs>()... };

			EntityComponentKey required_component_key = 0;

			for (auto &id : component_ids)
			{
				required_component_key |= EntityComponentKey{ id };
			}

			return required_component_key;
		}

		EntityComponentKey required_component_keys_ = GetRequiredComponentKey<T, Components...>();

	public:

		virtual ~System() {}

		virtual void TryInitialize(Entity * entity) override
		{
			if ((entity->component_key & required_component_keys_) == required_component_keys_)
			{
				static_cast<T*>(this)->Initialize(entity);
			}
		}

		virtual void TryClean(Entity * entity) override
		{
			if ((entity->component_key & required_component_keys_) == required_component_keys_)
			{
				static_cast<T*>(this)->Clean(entity);
			}
		}

		virtual void TryUpdate(Entity * entity) override
		{
			if ((entity->component_key & required_component_keys_) == required_component_keys_)
			{
				static_cast<T*>(this)->Update(entity);
			}
		}

		virtual void Initialize(Entity * entity) = 0;

		virtual void Update(Entity * entity) = 0;

		virtual void Clean(Entity * entity) = 0;

	protected:

		System() {}

	};
}