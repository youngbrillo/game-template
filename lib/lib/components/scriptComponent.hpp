#pragma once
#include "lib/core/entity.hpp"
#include "lib/scripting/luaScript.hpp"

namespace lib
{
	struct ScriptComponent
	{
        struct {
		    sol::function onInit;
		    sol::function onFree;
		    sol::function onUpdate;
        } functions;
		sol::table self;
		//sol::function fixedUpdateFunc;

		std::string path;
		bool valid = false;

		void init(Entity owner, sol::state& lua);
        void free();
        void update(float dt);


		void write(YAML::Emitter& out);
		void read(const YAML::Node& node);
		void inspect();
		static void Bind(sol::state& lua);
	};
}
