#include "sprite.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include "lib/utils/editor_utils.hpp"
#include <lib/resources/resource_manager.hpp>

namespace lib
{
	void Sprite::render(const Transform2D& p_t) const
	{
#ifdef USE_TEXTURE_HANDLE
		bool handle_is_valid = handle.Get() != nullptr;
		Texture2D texture = handle_is_valid ? handle->data : Texture2D{ 1,1,1 };
		DrawTexturePro(texture, getFrame(), p_t.getRectangle(), p_t.getOrigin(), p_t.orientation, tint);
#else
			DrawTexturePro(texture, getFrame(), p_t.getRectangle(), p_t.getOrigin(), p_t.orientation, tint);
#endif
	}
	void Sprite::serialize(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
#ifdef USE_TEXTURE_HANDLE
			<< YAML::Key << "id" << handle.GetUUID()
#else
			<< YAML::Key << "id" << id
#endif // USE_TEXTURE_HANDLE
			<< YAML::Key << "tint" << tint
			<< YAML::Key << "source" << source
			<< YAML::Key << "layer" << layer;
		if (flipX) out << YAML::Key << "flipX" << flipX;
		if (flipY) out << YAML::Key << "flipY" << flipY;
		out << YAML::EndMap;
	}

	void Sprite::deserialize(const YAML::Node& node)
	{
#ifdef USE_TEXTURE_HANDLE
		UUID id = 0;
		if (readYamlValue(node["id"], &id))
		{
			handle = TextureHandle(id, ResourceManager::GetSingleton().GetRegistry());
		}
#else
		readYamlValue(node["id"], &id);
#endif // USE_TEXTURE_HANDLE
		readYamlValue(node["tint"], &tint);
		readYamlValue(node["source"], &source);
		readYamlValue(node["layer"], &layer);
		readYamlValue(node["flipX"], &flipX);
		readYamlValue(node["flipY"], &flipY);
	}

	void Sprite::inspect()
	{
#ifdef USE_TEXTURE_HANDLE
		UUID id = handle.GetUUID();
		if (ResourceManager::GetSingleton().DrawTextureComboBox("texture", id, nullptr))
		{
			handle = TextureHandle(id, ResourceManager::GetSingleton().GetRegistry());
		}
#else
		ResourceManager::GetSingleton().DrawTextureComboBox("texture", id, &texture);
#endif // USE_TEXTURE_HANDLE
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
#ifdef USE_TEXTURE_HANDLE
#else
			, "id", &Sprite::id
			, "texture", &Sprite::texture
#endif // USE_TEXTURE_HANDLE
			, "tint", &Sprite::tint
			, "source", &Sprite::source
			, "layer", &Sprite::layer
			, "flipX", &Sprite::flipX
			, "flipY", &Sprite::flipY
		);
	}

}