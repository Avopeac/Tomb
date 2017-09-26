#pragma once

#include <string>
#include <unordered_map>

#include "scene_node.h"

namespace scene
{
	class SceneManager
	{

		SceneNode * current_scene_;

		std::unordered_map<std::string, std::unique_ptr<SceneNode>> scenes_;

	public:

		SceneManager();

		~SceneManager();

		void Update(float delta_time);

		SceneNode * Add(const std::string &name);

		SceneNode * Switch(const std::string &name);

		SceneNode * GetCurrent();

		SceneNode * GetNode(const std::string &name);

		SceneNode * GetNode(size_t id);
	};
}
