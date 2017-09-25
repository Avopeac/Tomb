#pragma once

#include <functional>
#include <iomanip>
#include "mapmodel.h"
#include "maplogic.h"
#include "renderer.h"
#include "resource_manager.h"

#include "grid.h"

namespace game
{
	class MapView
	{

		const MapModel &model_;

		const MapLogic &logic_;

		size_t hex_texture_hash_;
		size_t hex_mesh_hash_;

	public:

		MapView(const MapModel &model, const MapLogic &logic) :
			model_(model), 
			logic_(logic), 
			hex_texture_hash_(0),
			hex_mesh_hash_(0)
		{ 

		}

		~MapView() {}

		void Update(graphics::Renderer &renderer, float delta_time)
		{

			static float angle = 0.0f;
			angle += 500.0f * delta_time;

			if (hex_texture_hash_ == 0 && hex_mesh_hash_ == 0) 
			{
				auto &mesh_cache = graphics::ResourceManager::Get().GetMeshCache();
				auto &texture_cache = graphics::ResourceManager::Get().GetTextureCache();
				mesh_cache.GetFromFile("hex", "assets/models/hex_1.obj", &hex_mesh_hash_);
				texture_cache.GetFromFile("assets/textures/white_dot.png", true, &hex_texture_hash_);
			}
			
			glm::mat4 model =
				glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, -15.0f)) *
				glm::scale(glm::mat4(1), glm::vec3(20.0f));  
			 
			glm::mat4 rot_island = glm::rotate(glm::mat4(1), glm::mod((float)util::GetSeconds(), glm::two_pi<float>()), glm::vec3(0, 0, 1));

			for (auto hex_it = model_.GetTileBeginIterator(); hex_it != model_.GetTileEndIterator(); ++hex_it)
			{
				
				HexCoordinate hex = hex_it->first;
				glm::vec2 pos = logic_.AxialToCartesian(hex, 0.5f);

				glm::mat4 model =
					rot_island * 
					glm::translate(glm::mat4(1), glm::vec3(pos, -10.0f + (0.5f + 0.5f * (float)glm::sin(util::GetSeconds() + pos.x)))) *
					glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
					glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1, 0, 0)) *
					glm::scale(glm::mat4(1), glm::vec3(0.45f * glm::vec3(1.0f, 1.0f, 1.0f)));

				renderer.GetMeshRenderer().Push(hex_mesh_hash_, hex_texture_hash_, model);
			}
		}
	};
}