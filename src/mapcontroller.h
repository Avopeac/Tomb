#pragma once

#include "mapview.h"
#include "mapmodel.h"

namespace game
{
	class MapController
	{

		MapView &view_;
		MapModel &model_;

	public:

		MapController(MapView &view, MapModel &model) :
			view_(view), model_(model)
		{
		}

		~MapController() {}

	};
}