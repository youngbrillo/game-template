#pragma once
#include "lib/components/transform2d.hpp"

namespace lib
{
	struct SceneCamera2D {
		bool active = true;
		Camera2D camera = {
			.offset = {1280 / 2.0f,720 / 2.0f},
			.target = {0, 0},
			.rotation = 0,
			.zoom = 16,
		};

		operator const Camera2D& ()const {
			return camera;
		}

		void serialize(YAML::Emitter& out);
		void deserialize(const YAML::Node& node);
		void inspect();
		static void ScriptBind(sol::state& lua);
		inline static const char* TypeName() { return "SceneCamera2D"; }
		inline static const char* DisplayName() { return "Scene Camera 2D"; }
	};

	struct SceneCamera2DPan {
		bool enabled = true;
		bool lockMovementX = false;
		bool lockMovementY = false;
		Vector2 lastPosition;

		void update(Camera2D& camera);

		void serialize(YAML::Emitter& out);
		void deserialize(const YAML::Node& node);
		void inspect();
		static void ScriptBind(sol::state& lua);
		inline static const char* TypeName() { return "SceneCamera2DPan"; }
		inline static const char* DisplayName() { return "Scene Camera 2D Pan Controller"; }
		inline static void Update(SceneCamera2D& p_camera, SceneCamera2DPan& p_controller) {
			p_controller.update(p_camera.camera);
		}
	};

}