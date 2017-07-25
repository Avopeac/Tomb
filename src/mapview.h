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

			const float size = 16.0f;
			const float height = size * 2.0f;
			const float width = glm::sqrt(3.0f) * 0.5f * height;
			const float map_offset_x = 10.0f;
			const float map_offset_y = 15.0f;

			auto scale = glm::scale(glm::mat4(1), glm::vec3(width, height, 1));
			for (auto it = model_.GetTileBeginIterator(); it != model_.GetTileEndIterator(); ++it)
			{
				HexCoordinate hex = it->first;

				glm::vec3 position = glm::vec3(glm::vec2(map_offset_x, map_offset_y) + logic_.AxialToCartesian(hex, size), 0);

				auto transform = glm::translate(scale, position);

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
	};
}