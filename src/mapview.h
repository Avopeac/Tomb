#pragma once

#include <chrono>

#include "mapmodel.h"
#include "maplogic.h"
#include "renderer.h"

namespace game
{
	class MapView
	{

		const MapModel &model_;

		const MapLogic &logic_;

	public:

		MapView(const MapModel &model, const MapLogic &logic) :
			model_(model), logic_(logic)
		{
		}

		~MapView() {}

		void Update(graphics::Renderer &renderer, float delta_time)
		{
			/*for (auto it = model_.GetTileBeginIterator(); it != model_.GetTileEndIterator(); ++it)
			{
				HexCoordinate coordinate = it->first;
			}*/

			static bool upload = true;
			if (upload)
			{
				auto scale_mat = glm::scale(glm::mat4(1), glm::vec3(32, 32, 1));
				for (auto y = 0; y < 32; ++y)
				{
					for (auto x = 0; x < 32; ++x)
					{
						float x_offset = y % 2 == 0 ? 0.5f : -0.5f;
						auto transform = glm::translate(scale_mat,
							glm::vec3(x_offset + 2.0f * x, 1.5f * y, 0));

						graphics::Sprite sprite(transform, 0);
						renderer.GetSpriteRenderer().Push(sprite,
							graphics::SpriteShape::SharpHex,
							"assets/textures/sand/sand_08.png",
							graphics::BlendMode::SrcAlpha,
							graphics::BlendMode::OneMinusSrcAlpha,
							graphics::BlendMode::SrcAlpha,
							graphics::BlendMode::OneMinusSrcAlpha,
							graphics::MagnificationFiltering::Linear,
							graphics::MinificationFiltering::LinearMipmapLinear,
							graphics::Wrapping::ClampToEdge,
							graphics::Wrapping::ClampToEdge);
					}
				}

				upload = false;
			}
		}
	};
}