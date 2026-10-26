#pragma once
#include "collider3d_types.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include <imgui.h>

namespace lib
{
	void iCollider3D::onWrite(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "density" << density
			<< YAML::Key << "friction" << friction
			<< YAML::Key << "restitution" << restitution
			<< YAML::Key << "isSensor" << isSensor
			<< YAML::Key << "contactFlags" << contactFlags
			<< YAML::EndMap;
	}
	void iCollider3D::onRead(const YAML::Node& node)
	{
		if (node)
		{
			readYamlValue(node["density"], &density);
			readYamlValue(node["friction"], &friction);
			readYamlValue(node["restitution"], &restitution);
			readYamlValue(node["isSensor"], &isSensor);
			readYamlValue(node["contactFlags"], &contactFlags);
		}
	}

	void BoxCollider3D::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap;
		out << YAML::Key << "autoSize" << autoSize;
		if(!autoSize)
			out << YAML::Key << "size" << size;
			out << YAML::Key << "base" << YAML::Value;
				this->onWrite(out);

		out << YAML::EndMap;
	}
	void BoxCollider3D::read(const YAML::Node& node)
	{
		readYamlValue(node["autoSize"], &autoSize);
		readYamlValue(node["size"], &size);
		this->onRead(node["base"]);
	}

	void SphereCollider3D::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap;
		out << YAML::Key << "radius" << radius;
		out << YAML::Key << "origin" << origin;
		out << YAML::Key << "base" << YAML::Value;
		this->onWrite(out);

		out << YAML::EndMap;
	}
	void SphereCollider3D::read(const YAML::Node& node)
	{
		readYamlValue(node["radius"], &radius);
		readYamlValue(node["origin"], &origin);
		this->onRead(node["base"]);
	}

	void iCollider3D::onInspect()
	{
	}
	void BoxCollider3D::inspect()
	{
		ImGui::Checkbox("auto size", &autoSize);
		ImGui::DragFloat3("size", &size.x, 0.1f);
		this->onInspect();
	}
	void BoxCollider3D::Inspect(Entity& e, BoxCollider3D& component)
	{
		component.inspect();
	}
	void SphereCollider3D::inspect()
	{
		ImGui::DragFloat("radius", &radius, 0.1f);
		ImGui::DragFloat3("origin", &origin.x, 0.1f);

		this->onInspect();
	}

	void SphereCollider3D::Inspect(Entity& e, SphereCollider3D& component)
	{
		component.inspect();
	}

	void iCollider3D::Bind(sol::state& lua)
	{
		namespace help = scripting::bind;
		help::register_meta_component<iCollider3D>();

		lua.new_usertype<iCollider3D>("iCollider3D"
			, "type_id", &entt::type_hash<iCollider3D>::value
			, sol::call_constructor
			, sol::factories([]() {return iCollider3D(); })
			, "density", &iCollider3D::density
			, "friction", &iCollider3D::friction
			, "restitution", &iCollider3D::restitution
			, "isSensor", &iCollider3D::isSensor
			, "contactFlags", &iCollider3D::contactFlags
		);
	}
}