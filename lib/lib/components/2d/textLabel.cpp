#include "textLabel.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include "lib/utils/editor_utils.hpp"
#include <imgui_stdlib.h>
#include <lib/resources/resource_manager.hpp>


namespace lib
{
	inline Vector2 TextLabel::getMeasurements() const
	{
			return MeasureTextEx(
#ifdef USE_FONT_HANDLE
				handle.Get() != nullptr ? handle.Get()->data : GetFontDefault(),
#else
				font,
#endif
				text.c_str(), size, spacing);
	}
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

	inline void TextLabel::render(const Transform2D& p_t) const
	{
		DrawTextPro(
#ifdef USE_FONT_HANDLE
			handle.Get() != nullptr  ? handle.Get()->data : GetFontDefault(),
#else
			font,
#endif
		text.c_str(), p_t.position, origin, p_t.orientation, size, spacing, tint);
	}

	void TextLabel::Render(const Transform2D& transform, const TextLabel& label)
	{
		label.render(transform);
	}

	void TextLabel::serialize(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
#ifdef USE_FONT_HANDLE
			<< YAML::Key << "id" << handle.GetUUID()
#else
			<< YAML::Key << "id" << id
#endif
			<< YAML::Key << "tint" << tint
			<< YAML::Key << "size" << size
			<< YAML::Key << "spacing" << spacing
			<< YAML::Key << "origin" << origin
			<< YAML::Key << "text" << text
			<< YAML::EndMap;
	}
	void TextLabel::deserialize(const YAML::Node& node)
	{
#ifdef USE_FONT_HANDLE
		UUID id = handle.GetUUID();
		if (readYamlValue(node["id"], &id))
		{
			handle = FontHandle(id, ResourceManager::GetSingleton().GetRegistry());
		}
#else
		readYamlValue(node["id"], &id);
#endif
		readYamlValue(node["tint"], &tint);
		readYamlValue(node["size"], &size);
		readYamlValue(node["spacing"], &spacing);
		readYamlValue(node["origin"], &origin);
		readYamlValue(node["text"], &text);

	}
	void TextLabel::inspect()
	{
#ifdef USE_FONT_HANDLE
		UUID id = handle.GetUUID();
		if (ResourceManager::GetSingleton().DrawFontComboBox("font", id, nullptr))
		{
			handle = FontHandle(id, ResourceManager::GetSingleton().GetRegistry());
		}
#else
		ResourceManager::GetSingleton().DrawFontComboBox("font", id, &font);
#endif

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