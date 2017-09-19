#pragma once

#include "scene_node.h"

namespace graphics
{
	class SceneManager
	{

		SceneNode * root_ = nullptr;

	public:

		SceneManager();

		~SceneManager();

		SceneNode &GetRoot() { return *root_; }

	};
}
