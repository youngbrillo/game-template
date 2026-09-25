#pragma once
#include "transform3d.hpp"

namespace lib
{
	struct SceneCamera3D
	{
		Camera3D camera = Camera3D{
			.position = {10, 10, 10},
			.target = {0, 0, 0},
			.up = {0,1,0},
			.fovy = 45,
			.projection = CameraProjection::CAMERA_PERSPECTIVE
		};

		inline Vector3 forward() const {
			return Vector3Normalize(Vector3{
				camera.target.x - camera.position.x, 0,
				camera.target.z - camera.position.z
				});
		}

		inline Vector3 right() const {
			Vector3 f = forward();
			return Vector3Normalize(Vector3{ f.z, 0.0f, -f.x });
		}

		inline Vector2 GetRelativeDirection(Vector2 p_inputDirection) const
		{
			Vector2 input = p_inputDirection;
			Vector3 camForward = forward();
			Vector3 camRight = right();

			Vector3 i = (camForward * input.y) + (camRight * input.x);
			input = Vector2{ -i.x, -i.z };
			return input;
		}

		operator Camera3D() const { return camera; }

		void write(YAML::Emitter& out);
		void read(const YAML::Node& node);
		void inspect();
		static void Bind(sol::state& lua);
	};

}