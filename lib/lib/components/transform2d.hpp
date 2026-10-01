#pragma once
#include <raylib.h>
#include <raymath.h>
#include "lib/utils/yaml_common.hpp"
#include "sol/sol.hpp"

namespace lib
{
	struct Transform2D
	{
		Vector2 position = { 0, 0 };
		Vector2 size = { 1, 1 };
		Vector2 origin = { 0.5f, 0.5f };
		float orientation = 0.0f;

		Transform2D() {};
		Transform2D(float x, float y) : position(Vector2{ x, y }) {}
		Transform2D(Vector2 v) : position(v) {};
		Transform2D(Vector2 v, Vector2 s) : position(v), size(s) {};
		Transform2D(const Transform2D& o) = default;
		//Transform2D(const Matrix3x3& m);
		//Matrix3x3 GetMatrix() const;

		inline Rectangle	getRectangle() const { return Rectangle{ position.x, position.y, size.x, size.y }; }
		inline Rectangle	getAnchoredRectangle() const{
			Rectangle dest = getRectangle();
			Vector2 origin = getOrigin();
			dest.x -= origin.x;
			dest.y -= origin.y;
			return dest;
		}
		inline Vector2 getOrigin() const  { return size * origin; }
		inline Vector2 getHeading() const { return Vector2{ sinf(orientation * DEG2RAD), cosf(orientation * DEG2RAD) };}


		void serialize(YAML::Emitter& out);
		void deserialize(const YAML::Node& node);
		bool inspect();
		static void ScriptBind(sol::state& lua);
		inline static const char* TypeName() { return "Transform2D"; }
		inline static const char* DisplayName() { return "Transform 2D"; }

	};
}