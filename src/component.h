#pragma once

namespace entity {

	class Component
	{

	public:

		virtual ~Component() {}

		template <typename T> static size_t GetId()
		{
			static_assert(std::is_base_of<Component, T>::value,
				"Derived class must be of base type Component.");

			static size_t type_id;
			static uint8_t instance_created = false;

			if (!instance_created)
			{
				type_id = GetNewId();
				instance_created = true;
			}

			return type_id;
		}

	protected:

		Component() {}

	private:

		static size_t GetNewId()
		{
			static size_t id = 1;
			return id++;
		}
	};
}
