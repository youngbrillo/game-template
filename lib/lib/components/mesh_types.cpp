#include "mesh_types.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"

namespace lib
{
	void StaticMesh::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "id" << id
			<< YAML::Key << "tint" << tint
			<< YAML::EndMap;
	}

	void StaticMesh::read(const YAML::Node& node)
	{
		readYamlValue(node["id"], &id);
		readYamlValue(node["tint"], &tint);
	}

	void StaticMesh::inspect()
	{
	}

	void StaticMesh::Bind(sol::state& lua)
	{
		namespace help = scripting::bind;
		help::register_meta_component<StaticMesh>();

		lua.new_usertype<StaticMesh>("StaticMesh"
			, "type_id", &entt::type_hash<StaticMesh>::value
			, sol::call_constructor
			, sol::factories([]() {return StaticMesh(); })
			, "id", &StaticMesh::id
			, "tint", &StaticMesh::tint
		);
	}

	void StaticMesh::Inspect(Entity e, StaticMesh& component)
	{
		component.inspect();
	}
}