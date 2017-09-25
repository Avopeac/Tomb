#pragma once

#include <string>
#include <vector>

#include "LuaJIT-2.0.5/src/lua.hpp"
#include "LuaJIT-2.0.5/src/lualib.h"

#include "logger.h"

namespace script {

	class LuaScript
	{
		int level_;

		lua_State * lua_state_;

	public:

		LuaScript(const std::string &path) :
			level_(0),
			lua_state_(luaL_newstate())
		{
			if (luaL_loadfile(lua_state_, path.c_str()) || lua_pcall(lua_state_, 0, 0, 0))
			{
				std::string error = "Error loading Lua script from " + path + ".";
				debug::Log(SDL_LOG_PRIORITY_WARN, SDL_LOG_CATEGORY_INPUT, error.c_str());
				lua_close(lua_state_);
			}
		}

		~LuaScript()
		{
			if (lua_state_)
			{
				lua_close(lua_state_);
			}
		}

		template <typename T> T get(const std::string &name)
		{
			if (!lua_state_)
			{
				std::string error = "Error loading " + name + ", Lua script not loaded.";
				debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_INPUT, error.c_str());
				return getLuaDefault<T>();
			}

			T result = getToLuaStack(name) ? getLua<T>(name) : getLuaDefault<T>();
			
			lua_pop(lua_state_, level_ + 1);

			return result;
		}

		bool getToLuaStack(const std::string &name)
		{
			level_ = 0;

			std::string variable = "";

			for (size_t i = 0; i < name.size(); ++i)
			{
				if (name[i] == '.')
				{
					level_ == 0 ? lua_getglobal(lua_state_, variable.c_str()) :
						lua_getfield(lua_state_, -1, variable.c_str());

					if (lua_isnil(lua_state_, -1))
					{
						std::string error = "Variable " + variable + " is not defined in Lua script.";
						debug::Log(SDL_LOG_PRIORITY_WARN, SDL_LOG_CATEGORY_INPUT, error.c_str());
						return false;
					}
					else
					{
						variable = "";
						level_++;
					}
				}
				else
				{
					variable += name[i];
				}
			}

			level_ == 0 ? lua_getglobal(lua_state_, variable.c_str()) :
				lua_getfield(lua_state_, -1, variable.c_str());

			if (lua_isnil(lua_state_, -1))
			{
				std::string error = "Variable " + variable + " is not defined in Lua script.";
				return false;
			}

			return true;
		}

		template <typename T> T getLuaDefault()
		{
			return 0;
		}

		template <> inline std::string getLuaDefault()
		{
			return "null";
		}

		template <typename T> T getLua(const std::string &name)
		{
			return 0;
		}

		template <> inline bool getLua(const std::string &name)
		{
			return (bool)lua_toboolean(lua_state_, -1);
		}

		template <> inline float getLua(const std::string &name)
		{
			if (!lua_isnumber(lua_state_, -1))
			{
				std::string error = "Variable " + name + " is not a number.";
				debug::Log(SDL_LOG_PRIORITY_WARN, SDL_LOG_CATEGORY_INPUT, error.c_str());
			}
			
			return (float)lua_tonumber(lua_state_, -1);
		}

		template <> inline std::string getLua(const std::string &name)
		{
			std::string str = "null";
			if (lua_isstring(lua_state_, -1))
			{
				str = std::string(lua_tostring(lua_state_, -1));
			}
			else
			{
				std::string error = "Variable " + name + " is not a number.";
				debug::Log(SDL_LOG_PRIORITY_WARN, SDL_LOG_CATEGORY_INPUT, error.c_str());
			}
			
			return str;
		}

		std::vector<float> getNumbers(const std::string &name)
		{
			std::vector<float> numbers;
			lua_getglobal(lua_state_, name.c_str());
			if (lua_isnil(lua_state_, -1))
			{
				return std::vector<float>();
			}

			lua_pushnil(lua_state_);
			while (lua_next(lua_state_, -2))
			{
				numbers.push_back((float)lua_tonumber(lua_state_, -1));
				lua_pop(lua_state_, 1);
			}

			clean();
			return numbers;
		}

		std::vector<std::string> getStrings(const std::string &name)
		{
			std::vector<std::string> strings;
			lua_getglobal(lua_state_, name.c_str());
			if (lua_isnil(lua_state_, -1))
			{
				return std::vector<std::string>();
			}

			lua_pushnil(lua_state_);
			while (lua_next(lua_state_, -2))
			{
				strings.push_back(lua_tostring(lua_state_, -1));
				lua_pop(lua_state_, 1);
			}

			clean();
			return strings;
		}

		std::vector<uint8_t> getBooleans(const std::string &name)
		{
			std::vector<uint8_t> booleans;
			lua_getglobal(lua_state_, name.c_str());
			if (lua_isnil(lua_state_, -1))
			{
				return std::vector<uint8_t>();
			}

			lua_pushnil(lua_state_);
			while (lua_next(lua_state_, -2))
			{
				booleans.push_back((uint8_t)lua_toboolean(lua_state_, -1));
				lua_pop(lua_state_, 1);
			}

			clean();
			return booleans;
		}

		void clean()
		{
			int items = lua_gettop(lua_state_);
			lua_pop(lua_state_, items);
		}
	};
}
