#pragma once
#include "transform2d.hpp"
#include "lib/core/uuid.hpp"

namespace lib
{
	struct TextLabel
	{
		UUID id = 10;
		Font font = GetFontDefault();
		Color tint = WHITE;
		float size = 20.0f;
		float spacing = 1.0f;
		Vector2 origin = {0, 0};
		std::string text = "label";

		inline Vector2 getMeasurements() const
		{
			return MeasureTextEx(font, text.c_str(), size, spacing);
		}

		void center();
		void leftAlign();
		void rightAlign();

		inline void render(const Transform2D& p_t) const {
			DrawTextPro(
				font, text.c_str(), p_t.position, origin, p_t.orientation, size, spacing, tint);
		}
		inline static void Render(const Transform2D& transform, const TextLabel& label) { label.render(transform); }

		void serialize(YAML::Emitter& out);
		void deserialize(const YAML::Node& node);
		void inspect();
		static void ScriptBind(sol::state& lua);
		inline static const char* TypeName() { return "TextLabel"; }
		inline static const char* DisplayName() { return "Text Label"; }
	};
}