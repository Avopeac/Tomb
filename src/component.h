#pragma once

namespace entity {

	constexpr size_t max_defined_components = 1 << 8;

	class Entity;

	class Component
	{
		
		Entity * entity_;

	public:

		virtual ~Component() {}

		template <typename T> static size_t GetTypeId()
		{
			static_assert(std::is_base_of<Component, T>::value,
				"Derived class must be of base type Component.");

			static size_t type_id;
			static uint8_t instance_created = false;

			if (!instance_created)
			{
				type_id = GetNewTypeId();
				instance_created = true;
			}

			return type_id;
		}

		inline const Entity * const GetEntity() const { return entity_; }

	protected:

		Component(Entity * entity) : entity_(entity) {}
		
	private:

		static size_t GetNewTypeId()
		{
			static size_t uid = 1;
			return uid++;
		}
	};
}
