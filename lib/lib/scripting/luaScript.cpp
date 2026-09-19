#include "lib/scripting/luaScript.hpp"
#include "lib/scripting/luaScriptBindings.hpp"
#include <raylib.h>

namespace lib
{
	void LuaFunction_CatchError(const sol::error& error)
	{
		TraceLog(TraceLogLevel::LOG_ERROR, "[LuaScript]:\t%s", error.what());
	}

	void LuaScript::init()
	{
		if (isEnabled())
		{
			TraceLog(TraceLogLevel::LOG_INFO, "[LuaScript]: Script already enabled");
		}
		else
		{
			scripting::BindAll(state);
			TraceLog(TraceLogLevel::LOG_INFO, "[LuaScript]: Script initialized");
		}
	}

	void LuaScript::free()
	{
		self = sol::table();
		state = sol::state();
	}

	bool LuaScript::LoadFile(const std::string& filepath)
	{
		init();
		bool rValue = false;
		try {
			sol::protected_function_result executable = state.safe_script_file(filepath);
			if (executable.valid())
			{
				self = executable;
				rValue = true;
			}
		}
		catch (const sol::error& e)
		{
			LuaFunction_CatchError(e);
		}
		return rValue;
	}
}