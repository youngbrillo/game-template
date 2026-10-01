#pragma once
#include "lib/components/transform2d.hpp"
#include "lib/core/uuid.hpp"

namespace lib
{
	struct Sprite
	{
		UUID id = 10;
		Texture2D texture = { 1, 1, 1, 7 };
		Color tint = WHITE;
		Rectangle source = { 0, 0, 32, 32 };
		int layer = 0;
		bool flipX = false;
		bool flipY = false;

		inline Rectangle getFrame() const
		{
			Rectangle r = source;
			r.width *= flipX ? -1 : 1;
			r.height *= flipY ? -1 : 1;
			return r;
		}

		inline void render(const Transform2D& p_t) const {
			DrawTexturePro(texture, getFrame(), p_t.getRectangle(), p_t.getOrigin(), p_t.orientation, tint);}
		inline static void Render(const Transform2D& transform, const Sprite& sprite) { sprite.render(transform); }

		void serialize(YAML::Emitter& out);
		void deserialize(const YAML::Node& node);
		void inspect();
		static void ScriptBind(sol::state& lua);
		inline static const char* TypeName() { return "Sprite"; }
		inline static const char* DisplayName() { return "Sprite"; }
	};
}