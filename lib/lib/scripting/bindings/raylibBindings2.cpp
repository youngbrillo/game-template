#include "lib/scripting/luaScriptBindings.hpp"
#include <raylib.h>
#include <raymath.h>
#define CMP_EPSILON				0.00001

namespace lib::scripting
{
	static bool is_zero_approx(float p_value) {
		return abs(p_value) < (float)CMP_EPSILON;
	}

	static bool Vector2_is_zero_Approx(const Vector2& p_value)
	{
		return is_zero_approx(p_value.x) && is_zero_approx(p_value.y);
	}
	static bool Vector3_is_zero_Approx(const Vector3& p_value)
	{
		return is_zero_approx(p_value.x)
			&& is_zero_approx(p_value.y)
			&& is_zero_approx(p_value.z);
	}


    void BindRaylibMathTypes(sol::state& lua)
    {
		lua.new_usertype<Vector2>("Vector2",
			sol::meta_function::construct,
			sol::factories(
				//Vector2.new()
				[]() {return Vector2{ 0.0f, 0.0f }; }, [](float x, float y) {return Vector2{ x, y }; },
				//Vector2:new()
				[](sol::object) {return Vector2{ 0.0f, 0.0f }; }, [](sol::object, float x, float y) {return Vector2{ x, y }; }
			),
			//Vector(x,y) syntax, only
			sol::call_constructor,
			sol::factories([](float x, float y) {return Vector2{ x, y }; })
			, "x", &Vector2::x
			, "y", &Vector2::y
			, "set", [](Vector2& v, float x, float y)  -> Vector2& {v.x = x; v.y = y; return v; }
		, "normalize", [](Vector2& v) -> Vector2& {v = Vector2Normalize(v); return v; }
		, "scale", [](Vector2& v, float s) -> Vector2& {v = Vector2Scale(v, s); return v; }
		, "DirectionTowards", [](const Vector2& v, Vector2 other) -> Vector2 { Vector2 r = Vector2Normalize(Vector2Subtract(other, v)); return r; }
		, "DistanceToSqr", [](const Vector2& v, Vector2 other) -> float {  float r = Vector2DistanceSqr(other, v); return r; }
		, "DistanceTo", [](const Vector2& v, Vector2 other) -> float {  float r = Vector2Distance(other, v); return r; }
			, "is_zero_approx", Vector2_is_zero_Approx
			, sol::meta_function::addition, [](const Vector2& a, const Vector2& b) { return a + b; }
		, sol::meta_function::subtraction, [](const Vector2& a, const Vector2& b) { return a - b; }
		, sol::meta_function::multiplication, [](const Vector2& a, const Vector2& b) { return a * b; }
		, sol::meta_function::multiplication, [](const Vector2& a, float v) { return a * v; }
			);

		lua.new_usertype<Vector3>("Vector3",
			sol::meta_function::construct,
			sol::factories(
				//Vector3.new()
				[]() {return Vector3{ 0.0f, 0.0f, 0.0f }; }, [](float x, float y) {return Vector3{ x, y }; },
				//Vector3:new()
				[](sol::object) {return Vector3{ 0.0f, 0.0f }; }, [](sol::object, float x, float y, float z) {return Vector3{ x, y, z }; }
			),
			//Vector(x,y,z) syntax, only
			sol::call_constructor,
			sol::factories([](float x, float y, float z) {return Vector3{ x, y,z }; })
			, "x", &Vector3::x
			, "y", &Vector3::y
			, "z", &Vector3::z
			, "set", [](Vector3& v, float x, float y, float z)  -> Vector3& {v.x = x; v.y = y; v.z = z; return v; }
		, "normalize", [](Vector3& v) -> Vector3& {v = Vector3Normalize(v); return v; }
		, "scale", [](Vector3& v, float s) -> Vector3& {v = Vector3Scale(v, s); return v; }
		, "DirectionTowards", [](const Vector3& v, Vector3 other) -> Vector3 { Vector3 r = Vector3Normalize(Vector3Subtract(other, v)); return r; }
		, "DistanceTo", [](const Vector3& v, Vector3 other) -> float {  float r = Vector3DistanceSqr(other, v); return r; }
			, "is_zero_approx", Vector3_is_zero_Approx
			, sol::meta_function::addition, [](const Vector3& a, const Vector3& b) { return a + b; }
		, sol::meta_function::subtraction, [](const Vector3& a, const Vector3& b) { return a - b; }
		, sol::meta_function::multiplication, [](const Vector3& a, const Vector3& b) { return a * b; }
		, sol::meta_function::multiplication, [](const Vector3& a, float v) { return a * v; }

			);

		lua.new_usertype<Vector4>("Vector4",
			sol::meta_function::construct,
			sol::factories(
				//Vector3.new()
				[]() {return Vector4{ 0.0f, 0.0f, 0.0f, 0.0f }; }, [](float x, float y, float z, float w) {return Vector4{ x, y, z, w }; },
				//Vector3:new()
				[](sol::object) {return Vector4{ 0.0f, 0.0f }; }, [](sol::object, float x, float y, float z, float w) {return Vector4{ x, y, z, w }; }
			),
			//Vector(x,y,z) syntax, only
			sol::call_constructor,
			sol::factories([](float x, float y, float z, float w) {return Vector4{ x, y, z, w }; }),
			"x", &Vector4::x,
			"y", &Vector4::y,
			"z", &Vector4::z,
			"w", &Vector4::w,
			"set", [](Vector4& v, float x, float y, float z, float w)  -> Vector4& {v.x = x; v.y = y; v.z = z; v.w = w; return v; }
		);

    }
}