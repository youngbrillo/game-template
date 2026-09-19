#include "lib/scripting/luaScriptBindings.hpp"
#include <raylib.h>
#include <raymath.h>

namespace lib::scripting
{
    void BindRaylib(sol::state& lua)
    {
		lua.new_usertype<Rectangle>("Rectangle",
			sol::constructors<Rectangle()>()
			, sol::call_constructor
			, sol::factories([](float x, float y, float w, float h) {return Rectangle{ x, y, w, h }; })
			, "x", &Rectangle::x
			, "y", &Rectangle::y
			, "width", &Rectangle::width
			, "height", &Rectangle::height
			, "set", [](Rectangle& v, float x, float y, float w, float h) {v = Rectangle{ x, y, w, h }; }
		);

		lua.new_usertype<Color>("Color",
			sol::constructors<Color()>(),
			sol::call_constructor,
			sol::factories(
				[]() {return Color{ 255,255,255,255 }; }
		, [](unsigned int hex) {return GetColor(hex); }
			),
			"r", &Color::r,
			"g", &Color::g,
			"b", &Color::b,
			"a", &Color::a
			, "set", [](Color& c, unsigned int hex) { c = GetColor(hex); }
		, "alpha", [](Color& c, float alpha) { c = ColorAlpha(c, alpha); }
		, "lerp", [](Color& c, Color b, float factor) { c = ColorLerp(c, b, factor); }
			);

		lua.new_usertype<Camera2D>("Camera2D"
			, "offset", &Camera2D::offset
			, "target", &Camera2D::target
			, "rotation", &Camera2D::rotation
			, "zoom", &Camera2D::zoom
		);

		lua.new_usertype<Camera3D>("Camera3D"
			, "position", &Camera3D::position
			, "target", &Camera3D::target
			, "up", &Camera3D::up
			, "fovy", &Camera3D::fovy
			, "projection", &Camera3D::projection
		);

		lua.new_usertype<Texture>("Texture"
			, "id", &Texture::id
			, "width", &Texture::width
			, "height", &Texture::height
			, "mipmaps", &Texture::mipmaps
			, "format", &Texture::format
		);

		lua.new_usertype<Font>("Font"
			, "texture", &Font::texture
		);
		lua.new_usertype<Sound>("Sound"
			, "frameCount", &Sound::frameCount
		);
		lua.new_usertype<Music>("Music"
			, "frameCount", &Music::frameCount
			, "looping", &Music::looping
		);
		//input: keyboard
		lua["IsKeyPressed"] = IsKeyPressed;
		lua["IsKeyPressedRepeat"] = IsKeyPressedRepeat;
		lua["IsKeyDown"] = IsKeyDown;
		lua["IsKeyReleased"] = IsKeyReleased;
		lua["IsKeyUp"] = IsKeyUp;
		lua["GetKeyPressed"] = GetKeyPressed;
		lua["GetScreenWidth"] = GetScreenWidth;
		lua["GetScreenHeight"] = GetScreenHeight;

		//input: Gamepad

		lua["IsGamepadAvailable"] = IsGamepadAvailable;
		lua["GetGamepadName"] = GetGamepadName;
		lua["IsGamepadButtonPressed"] = IsGamepadButtonPressed;
		lua["IsGamepadButtonDown"] = IsGamepadButtonDown;
		lua["IsGamepadButtonReleased"] = IsGamepadButtonReleased;
		lua["IsGamepadButtonUp"] = IsGamepadButtonUp;
		lua["GetGamepadButtonPressed"] = GetGamepadButtonPressed;
		lua["GetGamepadAxisCount"] = GetGamepadAxisCount;
		lua["GetGamepadAxisMovement"] = GetGamepadAxisMovement;
		lua["SetGamepadMappings"] = SetGamepadMappings;
		lua["SetGamepadVibration"] = SetGamepadVibration;

		//resources
		lua["LoadTexture"] = LoadTexture;
		lua["UnloadTexture"] = UnloadTexture;



		lua["Trace"] = [](const char* message) {TraceLog(LOG_TRACE, message); };
		lua["Debug"] = [](const char* message) {TraceLog(LOG_DEBUG, message); };
		lua["Log"] = [](const char* message) {TraceLog(LOG_INFO, message); };
		lua["Warn"] = [](const char* message) {TraceLog(LOG_WARNING, message); };
		lua["Error"] = [](const char* message) {TraceLog(LOG_ERROR, message); };
		lua["FATAL"] = [](const char* message) {TraceLog(LOG_FATAL, message); };

    }
}