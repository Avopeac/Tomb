#pragma once

#include <memory>
#include <unordered_map>

namespace graphics
{
	class SceneNode
	{
		size_t id_ = SIZE_MAX;

		SceneNode * parent_node_ = nullptr;

		std::unordered_map<size_t, SceneNode *> child_nodes_;

	public:

		SceneNode();

		virtual ~SceneNode();

		bool RemoveChildren();

		void RemoveChild(SceneNode * child_node);

		void RemoveChild(size_t child_id);

		void AddChild(SceneNode * child_node);

		void Update(float delta_time);

	};
}
