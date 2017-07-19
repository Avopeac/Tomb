#pragma once

#include "mapmodel.h"
#include "renderer.h"

namespace game
{
	class MapView
	{

		const MapModel &model_;

	public:

		MapView(const MapModel &model) :
			model_(model)
		{
		}

		~MapView() {}

		void Update(graphics::Renderer &renderer, float delta_time)
		{
		}
	};
}