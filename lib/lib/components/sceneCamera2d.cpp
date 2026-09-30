#include "sceneCamera2d.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include <imgui.h>

namespace lib
{

	void SceneCamera2DPan::update(Camera2D& camera)
	{
        if (enabled == false)
            return;

        int zoff = (int)GetMouseWheelMove();
        if (zoff && IsMouseButtonDown(MOUSE_BUTTON_RIGHT))//&& !ImGui::GetIO().WantCaptureMouse)
        {
            if (zoff > 0)
                camera.zoom *= 1.1f;
            else
                camera.zoom /= 1.1f;

            if (camera.zoom < 0.01f)
                camera.zoom = 0.01f;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            lastPosition = GetScreenToWorld2D(GetMousePosition(), camera);
        }
        else if (!IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            Vector2 wc = GetScreenToWorld2D(GetMousePosition(), camera);
            Vector2 mpd = wc - lastPosition;
            if (lockMovementX)
                mpd.x = 0;
            if (lockMovementY)
                mpd.y = 0;

            camera.target -= mpd;
        }
	}

	void SceneCamera2D::serialize(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "active" << active
			<< YAML::Key << "camera" << camera
			<< YAML::EndMap;
	}

	void SceneCamera2D::deserialize(const YAML::Node& node)
	{
		readYamlValue(node["active"], &active);
		readYamlValue(node["camera"], &camera);
	}

	void SceneCamera2D::inspect()
	{
		ImGui::Checkbox("active", &active);
		ImGui::DragFloat2("offset", &camera.offset.x, 0.1f);
		ImGui::DragFloat2("target", &camera.target.x, 0.1f);
		ImGui::DragFloat("rotation", &camera.rotation, 0.1f);
		ImGui::DragFloat("zoom", &camera.zoom, 0.1f);
	}

	void SceneCamera2DPan::serialize(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap;
		out << YAML::Key << "enabled" << enabled;
		if(lockMovementX) out << YAML::Key << "lockMovementX" << lockMovementX;
		if(lockMovementY) out << YAML::Key << "lockMovementY" << lockMovementY;
		out << YAML::EndMap;
	}

	void SceneCamera2DPan::deserialize(const YAML::Node& node)
	{
		readYamlValue(node["enabled"], &enabled);
		readYamlValue(node["lockMovementX"], &lockMovementX);
		readYamlValue(node["lockMovementY"], &lockMovementY);
	}

	void SceneCamera2DPan::inspect()
	{
		ImGui::Checkbox("enabled", &enabled);
		ImGui::Checkbox("lockMovementX", &lockMovementX);
		ImGui::Checkbox("lockMovementY", &lockMovementY);

	}

	void SceneCamera2D::ScriptBind(sol::state& lua)
	{
		namespace help = scripting::bind;
		help::register_meta_component<Transform2D>();;

		lua.new_usertype<SceneCamera2D>("SceneCamera2D"
			, "type_id", &entt::type_hash<SceneCamera2D>::value
			, sol::call_constructor
			, sol::factories([]() {return SceneCamera2D(); })
			, "active", &SceneCamera2D::active
			, "camera", &SceneCamera2D::camera
		);
	}

	void SceneCamera2DPan::ScriptBind(sol::state& lua)
	{
		lua.new_usertype<SceneCamera2DPan>("SceneCamera2DPan"
			, "type_id", &entt::type_hash<SceneCamera2DPan>::value
			, sol::call_constructor
			, sol::factories([]() {return SceneCamera2DPan(); })
			, "enabled", &SceneCamera2DPan::enabled
			, "lockMovementX", &SceneCamera2DPan::lockMovementX
			, "lockMovementY", &SceneCamera2DPan::lockMovementY
		);
	}
}