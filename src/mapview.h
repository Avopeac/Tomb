#pragma once

#include "mapmodel.h"
#include "maplogic.h"
#include "entity_manager.h"
#include "mesh_component.h"

namespace game
{
	class MapView
	{
		const MapModel &model_;
		const MapLogic &logic_;

		const std::string mesh_name_ = "hex";
		const std::string mesh_path_ = "assets/models/hex_1.obj";
		const std::string texture_path_ = "assets/textures/white_dot.png";

	public:

		MapView(const MapModel &model, const MapLogic &logic) :
			model_(model), 
			logic_(logic)
		{ 

			glm::mat4 transform = glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
				glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1, 0, 0)) *
				glm::scale(glm::mat4(1), glm::vec3(0.45f * glm::vec3(1.0f, 1.0f, 1.0f)));

			auto &entity_manager = entity::EntityManager::Get();

			size_t tile_counter = 0;
			for (auto &it = model.GetTileBeginIterator(); it != model.GetTileEndIterator(); ++it) 
			{
				auto * entity = entity_manager.CreateEntity("Tile" + std::to_string(tile_counter));
				auto * component = entity_manager.AddEntityComponent<entity::MeshComponent>(entity->id, mesh_name_,
					mesh_path_, texture_path_);
				
				HexCoordinate hex = it->first;
				glm::vec2 hex_position = logic_.AxialToCartesian(hex, 0.5f);

				glm::mat4 world_transform =	glm::translate(glm::mat4(1), glm::vec3(hex_position, -10.0f)) * transform;

				component->SetTransform(world_transform);
			}
		}

		~MapView() {}

		void Update(float delta_time)
		{

		}
	};
}