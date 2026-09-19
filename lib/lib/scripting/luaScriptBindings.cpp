#include "lib/scripting/luaScriptBindings.hpp"

void lib::scripting::BindAll(sol::state& lua)
{
	lua.open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::coroutine,
		sol::lib::string,
		sol::lib::os,
		sol::lib::math,
		sol::lib::math,
		sol::lib::table,
		sol::lib::debug,
		sol::lib::bit32,
		sol::lib::io
	);

	BindEntity(lua);
	BindLib(lua);
	BindRaylib(lua);
	BindRaylibMathTypes(lua);
	BindAppModules(lua);
}
