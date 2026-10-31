#include "textLabel.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include "lib/utils/editor_utils.hpp"
#include <imgui_stdlib.h>
#include <lib/resources/resource_manager.hpp>


namespace lib
{
	void TextLabel::center()
	{
		Vector2 measurements = getMeasurements();
		origin = measurements * 0.5f;
	}
	void TextLabel::leftAlign()
	{
		Vector2 measurements = getMeasurements();
		origin = measurements * Vector2{ 0, 0.5f };
	}
	void TextLabel::rightAlign()
	{
		Vector2 measurements = getMeasurements();
		origin = measurements * Vector2{ 1., 0.5f };
	}

	void TextLabel::serialize(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "id" << id
			<< YAML::Key << "tint" << tint
			<< YAML::Key << "size" << size
			<< YAML::Key << "spacing" << spacing
			<< YAML::Key << "origin" << origin
			<< YAML::Key << "text" << text
			<< YAML::EndMap;
	}
	void TextLabel::deserialize(const YAML::Node& node)
	{
		readYamlValue(node["id"], &id);
		readYamlValue(node["tint"], &tint);
		readYamlValue(node["size"], &size);
		readYamlValue(node["spacing"], &spacing);
		readYamlValue(node["origin"], &origin);
		readYamlValue(node["text"], &text);

	}
	void TextLabel::inspect()
	{
		ResourceManager::GetSingleton().DrawFontComboBox("font", id, &font);

		ImGui::ColorEditRaylib("tint", tint);
		ImGui::InputFloat("size", &size);
		ImGui::InputFloat("spacing", &spacing);
		ImGui::DragFloat2("origin", &origin.x);

		if (ImGui::Button("left Align")) leftAlign();
		ImGui::SameLine();
		if (ImGui::Button("center")) center();
		ImGui::SameLine();
		if (ImGui::Button("right Align")) rightAlign();

		ImGui::InputTextMultiline("text", &text);
	}
	void TextLabel::ScriptBind(sol::state& lua)
	{
	}
}