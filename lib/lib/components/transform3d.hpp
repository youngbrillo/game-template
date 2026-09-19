#pragma once
#include <raylib.h>
#include <raymath.h>

namespace YAML
{
	class Emitter;
	class Node;
}
namespace sol
{
	class state;
}
namespace lib
{
	struct Transform3D
	{
		Vector3 position = { 0, 0, 0 };

		Vector3 size = { 1,1,1 };

		Quaternion orientation = QuaternionIdentity();

		Transform3D() = default;

		Transform3D(Vector3 Position);

		Transform3D(Vector3 Position, Vector3 Size);

		Transform3D(Vector3 Position, Vector3 Size, Quaternion Orientation);

		Transform3D(Matrix mat4x4);

		Transform3D(const Transform3D& other) = default;

		Matrix toMatrix() const;

		BoundingBox toBoundingBox() const;

		Vector3 GetEuler() const;

		void SetEuler(Vector3 euler);

		Vector3 Front() const;

		Vector3 Right() const;

		Vector3 Up() const;

		Quaternion LookAt(Vector3 direction, Vector3 up_axis) const;

		Quaternion RotateTowards(Vector3 direction, Vector3 up_axis, float radian) const;

		void RotateAroundAxis(Vector3 rotation_axis, float deg_angle);


		void Serialize(YAML::Emitter& out);
		void Deserialize(const YAML::Node& node);
		static void ScriptBind(sol::state& lua);
	};
}