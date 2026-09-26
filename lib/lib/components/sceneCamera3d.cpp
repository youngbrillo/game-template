#include "sceneCamera3d.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include <imgui.h>

namespace lib
{
	void SceneCamera3D::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "camera" << YAML::Value << camera
			<< YAML::EndMap;
	}
	void SceneCamera3D::read(const YAML::Node& node)
	{
		readYamlValue(node["camera"], &camera);
	}
	void SceneCamera3D::inspect()
	{
		ImGui::DragFloat3("position", &camera.position.x);
		ImGui::DragFloat3("target", &camera.target.x);
		ImGui::DragFloat3("up", &camera.up.x);
		ImGui::DragFloat("fovy", &camera.fovy, 0.1f, 0.01f, 180.0f);

		ImGui::CheckboxFlags("Perspective", &camera.projection, CAMERA_PERSPECTIVE);
		ImGui::SameLine();
		ImGui::CheckboxFlags("Orthographic", &camera.projection, CAMERA_ORTHOGRAPHIC);
	}
	void SceneCamera3D::Bind(sol::state& lua)
	{
		namespace help = scripting::bind;
		help::register_meta_component<SceneCamera3D>();

		lua.new_usertype<SceneCamera3D>("SceneCamera3D"
			, "type_id", &entt::type_hash<SceneCamera3D>::value
			, sol::call_constructor
			, sol::factories([]() {return SceneCamera3D(); })
			, "camera", &SceneCamera3D::camera
		);
	}
	void CameraController3D::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "enabled" << YAML::Value << enabled
			<< YAML::Key << "mode" << YAML::Value << mode
			<< YAML::EndMap;
	}
	void CameraController3D::read(const YAML::Node& node)
	{
		readYamlValue(node["enabled"], &enabled);
		readYamlValue(node["mode"], &mode);

	}
	void CameraController3D::inspect()
	{
		ImGui::Checkbox("enabled", &enabled);

		if (ImGui::RadioButton("Free", mode == CAMERA_FREE)) mode = CAMERA_FREE;
		if (ImGui::RadioButton("Orbital", mode == CAMERA_ORBITAL)) mode = CAMERA_ORBITAL;
		if (ImGui::RadioButton("First Person", mode == CAMERA_FIRST_PERSON)) mode = CAMERA_FIRST_PERSON;
		if (ImGui::RadioButton("Third Person", mode == CAMERA_THIRD_PERSON)) mode = CAMERA_THIRD_PERSON;

	}
	void CameraController3D::Bind(sol::state& lua)
	{
		namespace help = scripting::bind;
		help::register_meta_component<CameraController3D>();

		lua.new_usertype<CameraController3D>("CameraController3D"
			, "type_id", &entt::type_hash<CameraController3D>::value
			, sol::call_constructor
			, sol::factories([]() {return CameraController3D(); })
			, "enabled", &CameraController3D::enabled
			, "mode", &CameraController3D::mode
		);
	}
}