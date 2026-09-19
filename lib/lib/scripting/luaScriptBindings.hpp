#pragma once
#include <sol/sol.hpp>
#include "lib/core/uuid.hpp"
#include <functional>
typedef std::function<void(sol::state&)> LUAScriptBindingFunction;

namespace lib::scripting
{
	void BindAll(sol::state& lua);
	void BindEntity(sol::state& lua);
	void BindLib(sol::state& lua);
	void BindRaylib(sol::state& lua);
	void BindRaylibMathTypes(sol::state& lua);
	void BindAppModules(sol::state& lua);

	//adds a custom binding, can be a module. can be anything
	UUID addCustomBinding(LUAScriptBindingFunction function);
	//removes a custom binding
	void removeCustomBinding(UUID id);
}

