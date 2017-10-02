#pragma once

#include "entity_types.h"

namespace entity 
{

	class AbstractSystem
	{
	public:

		virtual void TryInitialize(Entity * entity) = 0;

		virtual void TryUpdate(Entity * entity) = 0;

		virtual void TryClean(Entity * entity) = 0;

		virtual ~AbstractSystem() {}

	protected:

		AbstractSystem() {}
	};
}