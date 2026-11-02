#pragma once
#include "transform2d.hpp"
#include "lib/core/uuid.hpp"
#define USE_FONT_HANDLE
#ifdef USE_FONT_HANDLE
	#include "lib/resources/resource_manager.hpp"
#endif // USE_FONT_HANDLE


namespace lib
{
	struct TextLabel
	{
		UUID id = 10;
#ifdef USE_FONT_HANDLE
		FontHandle handle;
#else
		Font font = GetFontDefault();
#endif // USE_FONT_HANDLE
		Color tint = WHITE;
		float size = 20.0f;
		float spacing = 1.0f;
		Vector2 origin = {0, 0};
		std::string text = "label";

		inline Vector2 getMeasurements() const;
		void center();
		void leftAlign();
		void rightAlign();

		void render(const Transform2D& p_t) const;
		static void Render(const Transform2D& transform, const TextLabel& label);

		void serialize(YAML::Emitter& out);
		void deserialize(const YAML::Node& node);
		void inspect();
		static void ScriptBind(sol::state& lua);
		inline static const char* TypeName() { return "TextLabel"; }
		inline static const char* DisplayName() { return "Text Label"; }
	};
}