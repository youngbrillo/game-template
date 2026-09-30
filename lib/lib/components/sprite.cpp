#include "sprite.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include "lib/utils/editor_utils.hpp"

namespace lib
{
	void Sprite::serialize(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "id" << id
			<< YAML::Key << "tint" << tint
			<< YAML::Key << "source" << source
			<< YAML::Key << "layer" << layer;
		if (flipX) out << YAML::Key << "flipX" << flipX;
		if (flipY) out << YAML::Key << "flipY" << flipY;
		out << YAML::EndMap;
	}

	void Sprite::deserialize(const YAML::Node& node)
	{
		readYamlValue(node["id"], &id);
		readYamlValue(node["tint"], &tint);
		readYamlValue(node["source"], &source);
		readYamlValue(node["layer"], &layer);
		readYamlValue(node["flipX"], &flipX);
		readYamlValue(node["flipY"], &flipY);
	}

	void Sprite::inspect()
	{
		ImGui::ColorEditRaylib("tint", tint);
		ImGui::InputFloat4("source", &source.x);
		ImGui::Checkbox("flip x", &flipX);
		ImGui::SameLine();
		ImGui::Checkbox("flip y", &flipY);
		ImGui::InputInt("layer", &layer);
	}

	void Sprite::ScriptBind(sol::state& lua)
	{
		namespace help = scripting::bind;

		help::register_meta_component<Transform2D>();;

		lua.new_usertype<Sprite>("Sprite"
			, "type_id", &entt::type_hash<Sprite>::value
			, sol::call_constructor
			, sol::factories([]() {return Sprite(); })
			, "id", &Sprite::id
			, "texture", &Sprite::texture
			, "tint", &Sprite::tint
			, "source", &Sprite::source
			, "layer", &Sprite::layer
			, "flipX", &Sprite::flipX
			, "flipY", &Sprite::flipY
		);
	}

}