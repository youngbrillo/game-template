#include "scriptComponent.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include <imgui.h>
#include <imgui_stdlib.h>

namespace lib
{
	void lib::ScriptComponent::init(Entity owner, sol::state& lua)
	{
        try {
            sol::protected_function_result executable = lua.safe_script_file(path);
            if (executable.valid())
            {
                self = executable;

                functions.onInit= self["onInit"];
                functions.onFree= self["onFree"];
                functions.onUpdate= self["onUpdate"];

                self["handle"] = sol::readonly_property([owner] { return owner.getHandleInt32(); });
                self["owner"] = owner;
                self["this"] = owner;
                self["entity"] = owner;
                self["ScriptComponent"] = this;


                LuaFunction_Execute(functions.onInit, self);
            }

            valid = true;
        }
        catch (const sol::error& e)
        {
            LuaFunction_CatchError(e);
            valid = false;
        }
	}

	void lib::ScriptComponent::free()
	{
        LuaFunction_Execute(functions.onFree, self);

        functions.onInit = sol::function();
        functions.onFree = sol::function();
        functions.onUpdate = sol::function();

        self = sol::table();
	}

	void lib::ScriptComponent::update(float dt)
	{
        if (functions.onUpdate) {
            if (!LuaFunction_Execute(functions.onUpdate, self, dt))
            {
                functions.onUpdate = sol::function();
            }
        }
	}

    void ScriptComponent::write(YAML::Emitter& out)
    {
        out << YAML::Flow << YAML::BeginMap
            << YAML::Key << "path" << YAML::Value << path
            << YAML::EndMap;
    }

    void ScriptComponent::read(const YAML::Node& node)
    {
        readYamlValue(node["path"], &path);

    }

    void ScriptComponent::inspect()
    {
        ImGui::InputText("path", &path);

    }

	void lib::ScriptComponent::Bind(sol::state& lua)
	{
        namespace help = scripting::bind;
        help::register_meta_component<ScriptComponent>();

        lua.new_usertype<ScriptComponent>("ScriptComponent"
            , "type_id", &entt::type_hash<ScriptComponent>::value
            , sol::call_constructor
            , sol::factories([]() {return ScriptComponent(); })
            , "path", &ScriptComponent::path
            , "valid", &ScriptComponent::valid
            , "self", &ScriptComponent::self
            , "init", &ScriptComponent::init
        );
	}
}