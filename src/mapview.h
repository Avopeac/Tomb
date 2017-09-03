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

		size_t texture_hash_;

		size_t hex_mesh_hash_;


	public:

		MapView(const MapModel &model, const MapLogic &logic) :
			model_(model), logic_(logic), texture_hash_(0), hex_mesh_hash_(0)
		{
		}

		~MapView() {}

		void Update(graphics::Renderer &renderer, float delta_time)
		{
			if (texture_hash_ == 0 && hex_mesh_hash_ == 0)
			{
				renderer.GetMeshCache().GetFromFile(hex_mesh_hash_, "hex", "assets/models/hex.obj");
				renderer.GetTextureCache().GetFromFile(texture_hash_, "assets/textures/smiley.png");
			}

			for (int i = 0; i < 4; ++i)
			{
				glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(2.0f * i, 0, -5)) * glm::rotate(glm::mat4(1), glm::radians(45.0f), glm::vec3(1,0,0));
				renderer.GetMeshRenderer().Push(hex_mesh_hash_, texture_hash_, model, glm::vec4(1, 0, 1, 1));
			}

		}
	};
}