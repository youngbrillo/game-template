#include "mesh_types.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include "imgui.h"

namespace lib
{
	StaticMesh::StaticMesh()
	{
		handle = ModelHandle(1, ResourceManager::GetSingleton().GetRegistry());
	}

	void StaticMesh::render(const Transform3D& transform) const
	{
		if (visible && handle.Get())
		{
			handle->data.transform = transform.toMatrix();
			DrawModel(handle->data, Vector3Zeros, 1.0f, tint);
		}
	}

	void StaticMesh::Render(const Transform3D& transform, const StaticMesh& component)
	{ 
		component.render(transform);
	}

	void StaticMesh::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "visible" << visible
			<< YAML::Key << "id" << handle.GetUUID()
			<< YAML::Key << "tint" << tint
			<< YAML::EndMap;
	}

	void StaticMesh::read(const YAML::Node& node)
	{
		readYamlValue(node["visible"], &visible);
		UUID id = 0;
		if (readYamlValue(node["id"], &id))
		{
			handle = ModelHandle(id, ResourceManager::GetSingleton().GetRegistry());
		}
		readYamlValue(node["tint"], &tint);
	}

	void StaticMesh::inspect()
	{
		ImGui::Checkbox("visible", &visible);

		UUID id = handle.GetUUID();
		if (ResourceManager::GetSingleton().DrawModelComboBox("model", id, nullptr))
		{
			handle = ModelHandle(id, ResourceManager::GetSingleton().GetRegistry());
		}
		Vector4 color = ColorNormalize(tint);
		if (ImGui::ColorEdit4("color", &color.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_DisplayHex))
		{
			tint = ColorFromNormalized(color);
		}
	}

	void StaticMesh::Bind(sol::state& lua)
	{
		namespace help = scripting::bind;
		help::register_meta_component<StaticMesh>();

		lua.new_usertype<StaticMesh>("StaticMesh"
			, "type_id", &entt::type_hash<StaticMesh>::value
			, sol::call_constructor
			, sol::factories([]() {return StaticMesh(); })
			, "visible", &StaticMesh::visible
			, "tint", &StaticMesh::tint
		);
	}

	void StaticMesh::Inspect(Entity e, StaticMesh& component)
	{
		component.inspect();
	}
}