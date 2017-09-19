#pragma once

#include <memory>
#include <limits>
#include <unordered_map>

namespace scene
{
	class SceneNode
	{
		const size_t id_ = std::numeric_limits<size_t>().max();

		SceneNode * parent_node_ = nullptr;

		std::unordered_map<size_t, SceneNode *> child_nodes_;

	public:

		SceneNode(const std::string &name);

		virtual ~SceneNode();

		size_t AddChild(SceneNode * child_node);

		void RemoveChild(SceneNode * child_node);

		void RemoveChild(size_t child_id);

		void RemoveChildren();

		SceneNode * GetParent();

		SceneNode * GetChild(const std::string &name);

		SceneNode * GetChild(size_t id);

		void Update(float delta_time);

	};
}
