#pragma once

#include <string>
#include <array>
#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>

#include "SDL_assert.h"

#include "entity.h"
#include "system.h"

namespace entity {

	constexpr size_t max_entities = 10000;

	class EntityManager
	{

		size_t uid_counter_;

		std::queue<size_t> free_uids_;

		std::unordered_map<size_t, std::string> entity_name_map_;

		std::array<Entity *, max_entities> entities_;

		std::vector<std::unique_ptr<AbstractSystem>> systems_;

	public:

		static EntityManager &Get()
		{
			static EntityManager instance;
			return instance;
		}

		~EntityManager()
		{
			for (size_t i = 0; i < max_entities; ++i)
			{
				if (entities_[i])
				{
					delete entities_[i];
				}
			}
		}

		Entity * Create(const std::string &name)
		{

			SDL_assert(!(uid_counter_ >= max_entities && free_uids_.empty()));
		
			Entity * obj = nullptr;

			size_t uid;

			if (!free_uids_.empty())
			{
				uid = free_uids_.front();
				free_uids_.pop();
			}
			else
			{
				uid = uid_counter_++;
			}

			obj = new Entity(uid);
			entities_[uid] = obj;
			entity_name_map_[uid] = name;
			return obj;
		}

		const std::string * const GetNameByUid(size_t uid) const
		{
			
			return entity_name_map_.find(uid) != entity_name_map_.cend() ?
				&entity_name_map_.at(uid) : nullptr;
		}

		Entity * FindFirstByName(const std::string &name)
		{
			for (auto &it = entity_name_map_.cbegin(); it != entity_name_map_.cend(); ++it)
			{
				if (it->second == name)
				{
					return entities_[it->first];
				}
			}
		}

		size_t Delete(size_t uid)
		{
			for (size_t i = 0; i < uid; ++i)
			{
				if (entities_[i])
				{
					delete entities_[i];

					entity_name_map_.erase(i);

					free_uids_.push(uid);
				}
			}
		}

		AbstractSystem * AddSystem(AbstractSystem * system)
		{
			systems_.push_back(std::move(std::unique_ptr<AbstractSystem>{system}));
			return systems_.back().get();
		}

	private:

		EntityManager() {}
	};
}
