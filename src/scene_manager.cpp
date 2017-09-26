#include "scene_manager.h"

#include "logger.h"

using namespace scene;

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{

}

void SceneManager::Update(float delta_time)
{
	if (current_scene_)
	{
		current_scene_->Update(delta_time);
	}
}

SceneNode * SceneManager::Add(const std::string & name)
{
	if (scenes_.find(name) == scenes_.end())
	{
		scenes_.insert({ name, std::move(std::make_unique<SceneNode>("root")) });

		auto * scene = scenes_[name].get();
		if (!current_scene_)
		{
			current_scene_ = scene;
		}

		return scene;
	}

	debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_APPLICATION, 
		"Trying to add scene with a name that is already occupied.");

	return nullptr;
}

SceneNode * scene::SceneManager::Switch(const std::string & name)
{
	if (scenes_.find(name) != scenes_.end())
	{
		current_scene_ = scenes_[name].get();
	}

	return nullptr;
}

SceneNode * scene::SceneManager::GetCurrent()
{
	return current_scene_;
}

SceneNode * SceneManager::GetNode(const std::string & name)
{
	return GetNode(std::hash<std::string>{}(name));
}

SceneNode * SceneManager::GetNode(size_t id)
{
	return current_scene_->GetChild(id);
}
