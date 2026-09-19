#pragma once 
#ifndef SOL_ALL_SAFETIES_ON 
	#define SOL_ALL_SAFETIES_ON 1
#endif // !SOL_ALL_SAFETIES_ON 
#include <sol/sol.hpp> //includes "lua.h"
#include <string>

namespace lib
{
	void LuaFunction_CatchError(const sol::error& error);

	template<typename... Args>
	inline bool LuaFunction_Execute(sol::function input, Args&&... args);

	struct LuaScript
	{
		sol::state state;
		sol::table self;

		void init();

		void free();

		bool LoadFile(const std::string& filepath);

		//executes any global function within the current lua context ('state')
		template<typename... Args>
		inline bool Execute(sol::function input, Args&&... args)
		{
			sol::protected_function func = input;
			if (func)
			{
				auto result = func(std::forward<Args>(args)...);
				bool isValid = result.valid();
				if (!isValid)
				{
					LuaFunction_CatchError(result);
				}
				return isValid;
			}
			return false;
		}

		//executes any global function within the current lua context ('state')
		template<typename... Args>
		inline bool ExecuteGlobalFunction(std::string functionName, Args&&... args)
		{
			sol::function func = state[functionName];
			return Execute(func, std::forward<Args>(args)...);
		}

		//executes any function that belongs to 'self' in the current lua context ('state')
		template<typename... Args>
		inline bool ExecuteScriptFunction(std::string functionName, Args&&... args)
		{
			if (!self.valid())
				return false;

			sol::function func = self[functionName];
			return Execute(func, self, std::forward<Args>(args)...);
		}

		inline bool isEnabled() const {
			return state.lua_state() != nullptr
				&& self.lua_state() != nullptr
				;
		}
	};


	template<typename... Args>
	inline bool LuaFunction_Execute(sol::function input, Args&&... args)
	{
		sol::protected_function func = input;
		if (func)
		{
			auto result = func(std::forward<Args>(args)...);
			bool isValid = result.valid();
			if (!isValid)
			{
				LuaFunction_CatchError(result);
			}
			return isValid;
		}
		return false;
	}
}