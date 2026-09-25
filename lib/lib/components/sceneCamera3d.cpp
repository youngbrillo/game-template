#include "sceneCamera3d.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"

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
}