#include "scene_node.h"

#include <functional>

#include "logger.h"

using namespace scene;

SceneNode::SceneNode(const std::string &name) :
	id_(std::hash<std::string>{}(name))
{
	
}

SceneNode::~SceneNode()
{
	
}

void SceneNode::Update(float delta_time)
{
}

size_t SceneNode::AddChild(SceneNode * child_node)
{
	if (child_node)
	{
		if (child_nodes_.find(child_node->id_) == child_nodes_.end())
		{
			child_nodes_.insert({ child_node->id_, child_node });
			child_node->parent_node_ = this;
			return child_node->id_;
		}
		else
		{
			debug::Log(SDL_LOG_PRIORITY_WARN, SDL_LOG_CATEGORY_APPLICATION,
				"Attempting to add node to scene graph where another instance of that id already exists.");
		}
	}

	return std::numeric_limits<size_t>().max();
}

void SceneNode::RemoveChild(SceneNode * child_node)
{
	if (child_node)
	{
		RemoveChild(child_node->id_);
	}
}

void SceneNode::RemoveChild(size_t child_id)
{
	if (child_nodes_.find(child_id) != child_nodes_.end())
	{
		child_nodes_[child_id]->RemoveChildren();

		delete child_nodes_[child_id];

		child_nodes_.erase(child_id);
	}
}

void SceneNode::RemoveChildren()
{
	for (auto &it : child_nodes_)
	{
		it.second->RemoveChildren();

		delete it.second;
	}

	child_nodes_.clear();
}

SceneNode * SceneNode::GetParent()
{
	return parent_node_;
}

SceneNode * SceneNode::GetChild(const std::string & name)
{
	return GetChild(std::hash<std::string>{}(name));
}

SceneNode * SceneNode::GetChild(size_t id)
{
	SceneNode * node = nullptr;

	if (child_nodes_.find(id) == child_nodes_.end())
	{
		for (auto &it : child_nodes_)
		{
			SceneNode * temp = it.second->GetChild(id);
			if (temp)
			{
				node = temp;
				break;
			}
		}
	}
	else
	{
		node = child_nodes_[id];
	}

	return node;
}

