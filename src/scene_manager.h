#pragma once

#include "scene_node.h"

namespace scene
{
	class SceneManager
	{

		SceneNode * root_ = nullptr;

	public:

		SceneManager();

		~SceneManager();

		void Update(float delta_time);

		SceneNode &GetRoot() { return *root_; }

		SceneNode * GetNode(const std::string &name);

		SceneNode * GetNode(size_t id);
	};
}
