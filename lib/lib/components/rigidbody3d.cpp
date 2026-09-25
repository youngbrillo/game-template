#include "lib/components/rigidbody3d.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"

namespace lib
{
	void Rigidbody3D::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "type" << type
			<< YAML::Key << "gravityScale" << gravityScale
			<< YAML::Key << "motionLocks" << motionLocks
			<< YAML::EndMap;
	}
	void Rigidbody3D::read(const YAML::Node& node)
	{
		readYamlValue(node["type"], &type);
		readYamlValue(node["gravityScale"], &gravityScale);
		readYamlValue(node["motionLocks"], &motionLocks);
	}
	void Rigidbody3D::inspect()
	{
	}
	void Rigidbody3D::Bind(sol::state& lua)
	{
		namespace help = scripting::bind;
		help::register_meta_component<Rigidbody3D>();

		lua.new_usertype<Rigidbody3D>("Rigidbody3D"
			, "type_id", &entt::type_hash<Rigidbody3D>::value
			, sol::call_constructor
			, sol::factories([]() {return Rigidbody3D(); })
			,"type", &Rigidbody3D::type
			, "gravityScale", &Rigidbody3D::gravityScale
			, "motionLocks", &Rigidbody3D::motionLocks
		);

	}
	void Rigidbody3D::Inspect(Entity e, Rigidbody3D& component)
	{
		component.inspect();
	}
	void Rigidbody3D::FixedUpdate(Transform3D& p_transform, Rigidbody3D& p_rigidbody)
	{
		p_rigidbody.applyTransform(p_transform);
	}
}