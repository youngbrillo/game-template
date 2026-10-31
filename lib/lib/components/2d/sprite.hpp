#pragma once
#include "transform2d.hpp"
#include "lib/core/uuid.hpp"
//#define USE_TEXTURE_HANDLE
#ifdef USE_TEXTURE_HANDLE
	#include "lib/resources/resource_manager.hpp"
#endif // USE_TEXTURE_HANDLE


namespace lib
{
	struct Sprite
	{
#ifdef USE_TEXTURE_HANDLE
		TextureHandle handle;
#else
		UUID id = 10;
		Texture2D texture = { 1, 1, 1, 7 };
#endif
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

		void render(const Transform2D& p_t) const;
		inline static void Render(const Transform2D& transform, const Sprite& sprite) { sprite.render(transform); }

		void serialize(YAML::Emitter& out);
		void deserialize(const YAML::Node& node);
		void inspect();
		static void ScriptBind(sol::state& lua);
		inline static const char* TypeName() { return "Sprite"; }
		inline static const char* DisplayName() { return "Sprite"; }
	};
}