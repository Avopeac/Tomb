#pragma once

#include <functional>
#include "mapmodel.h"
#include "maplogic.h"
#include "renderer.h"

#include "grid.h"

namespace game
{
	class MapView
	{

		const MapModel &model_;

		const MapLogic &logic_;

		size_t hex_texture_hash_;
		size_t hex_mesh_hash_;
		size_t hex_vertex_hash_;
		size_t hex_fragment_hash_;

		size_t ocean_texture_hash_;
		size_t ocean_mesh_hash_;
		size_t ocean_vertex_hash_;
		size_t ocean_fragment_hash_;

	public:

		MapView(const MapModel &model, const MapLogic &logic) :
			model_(model), 
			logic_(logic), 
			hex_texture_hash_(0),
			hex_mesh_hash_(0),
			hex_vertex_hash_(0),
			hex_fragment_hash_(0),
			ocean_texture_hash_(0),
		    ocean_mesh_hash_(0),
		    ocean_vertex_hash_(0),
		    ocean_fragment_hash_(0)
		{
			

		}

		~MapView() {}

		void UpdateHexPillar(graphics::Program * vertex, graphics::Program * fragment)
		{
			glm::vec4 color{ 1,0,0,1 };
			vertex->SetUniform("u_color", (void*)glm::value_ptr(color));
		}

		void UpdateWater(graphics::Program * vertex, graphics::Program * fragment)
		{
			glm::vec4 color{ 0.01, 0.4, 0.99, 1 };
			float time = util::GetSeconds();
			vertex->SetUniform("u_color", (void*)glm::value_ptr(color));
			vertex->SetUniform("u_time", (void*)&time);   
		} 

		void Update(graphics::Renderer &renderer, float delta_time)
		{

			static float angle = 0.0f;
			angle += 500.0f * delta_time;

			if (hex_texture_hash_ == 0 && hex_mesh_hash_ == 0 && hex_vertex_hash_ == 0 && hex_fragment_hash_ == 0)
			{
				renderer.GetMeshCache().GetFromFile(hex_mesh_hash_, "hex", "assets/models/hex_1.obj");
				renderer.GetTextureCache().GetFromFile(hex_texture_hash_, "assets/textures/smiley.png");
				renderer.GetProgramCache().GetFromFile("mesh.vert", hex_vertex_hash_, GL_VERTEX_SHADER, "assets/shaders/mesh.vert");
				renderer.GetProgramCache().GetFromFile("mesh.frag", hex_fragment_hash_, GL_FRAGMENT_SHADER, "assets/shaders/mesh.frag");
			}
			
			if (ocean_texture_hash_ == 0 && ocean_mesh_hash_ == 0 && ocean_vertex_hash_ == 0 && ocean_fragment_hash_ == 0)
			{
				std::vector<graphics::MeshVertex> vertices;
				std::vector<Uint32> indices;
				graphics::Grid::GetMeshData(32, vertices, indices);
				renderer.GetMeshCache().GetFromData(ocean_mesh_hash_, "grid", indices, vertices);
				renderer.GetTextureCache().GetFromFile(ocean_texture_hash_, "assets/textures/smiley.png");
				renderer.GetProgramCache().GetFromFile("ocean_mesh.vert", ocean_vertex_hash_, GL_VERTEX_SHADER, "assets/shaders/ocean_mesh.vert");
				renderer.GetProgramCache().GetFromFile("ocean_mesh.frag", ocean_fragment_hash_, GL_FRAGMENT_SHADER, "assets/shaders/ocean_mesh.frag");
			}

			glm::mat4 rot_time = glm::rotate(glm::mat4(1), glm::radians(glm::two_pi<float>() * angle), glm::vec3(1, 0, 0));


			glm::mat4 model =
				glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, -10.0f)) *
				//rot_time *
				glm::scale(glm::mat4(1), glm::vec3(10.0f));  
			 
			renderer.GetMeshRenderer().Push(ocean_mesh_hash_, ocean_texture_hash_,  
				ocean_vertex_hash_, ocean_fragment_hash_, model,
				std::bind(&MapView::UpdateWater, this, std::placeholders::_1, std::placeholders::_2));  

			for (auto hex_it = model_.GetTileBeginIterator(); hex_it != model_.GetTileEndIterator(); ++hex_it)
			{
				HexCoordinate hex = hex_it->first;
				glm::vec2 pos = logic_.AxialToCartesian(hex, 0.5f);
				glm::mat4 model = 
					glm::translate(glm::mat4(1), glm::vec3(pos, -10.0f)) * 
					//rot_time *
					glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 0, 1)) *
					glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1, 0, 0)) * 
					glm::scale(glm::mat4(1), glm::vec3(0.49f) * glm::vec3(1.0f, 1.0f, 1.0f));

				renderer.GetMeshRenderer().Push(hex_mesh_hash_, hex_texture_hash_, 
					hex_vertex_hash_, hex_fragment_hash_, model, 
					std::bind(&MapView::UpdateHexPillar, this, std::placeholders::_1, std::placeholders::_2));
			}
		}
	};
}