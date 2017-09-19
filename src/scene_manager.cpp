#include "scene_manager.h"

using namespace scene;

SceneManager::SceneManager()
{
	root_ = new SceneNode("root");
}

SceneManager::~SceneManager()
{
	delete root_;
}

void SceneManager::Update(float delta_time)
{
	root_->Update(delta_time);
}

SceneNode * SceneManager::GetNode(const std::string & name)
{
	return GetNode(std::hash<std::string>{}(name));
}

SceneNode * SceneManager::GetNode(size_t id)
{
	return root_->GetChild(id);
}
