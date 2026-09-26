#pragma once
#include "lib/components/transform3d.hpp"
#include "lib/components/rigidbody3d.hpp"
#include "lib/components/sceneCamera3d.hpp"

namespace lib
{
	struct MoverContact
	{
		b3Pos point;
		b3ShapeId shapeId;
	};
	struct PlayerMoverController
	{
		static constexpr int m_planeCapacity = 8;
		//inputs
		Vector2 direction = { 0, 0 };
		Vector3 velocity = { 0, 0, 0 };
		//parameters
		float movementSpeed = 10.0f;
		float jumpSpeed = 5.0f;
		float maxSpeed = 6.0f;
		float minSpeed = 0.01f;
		float stopSpeed = 1.0f;
		float accelerate = 30.0f;
		float friction = 4.0f;
		float gravity = 15.0f;
		float sprintMultiplier = 1.5f;
		float pogoLength = 3.0f;
		//internals
		b3WorldTransform m_transform = { .p = {0}, .q = b3Quat_identity };
		b3Capsule m_capsule = { { 0.0f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, 0.3f };
		b3CollisionPlane m_planes[m_planeCapacity] = {};
		MoverContact m_planeExtras[m_planeCapacity] = {};

		int m_planeCount = 0;
		int m_totalIterations = 0;
		float m_pogoVelocity = 0.0f;
		bool m_onGround = false;
		bool m_sprint = false;
		// Transient
		b3ShapeId* m_ignoreShapeIds = nullptr;
		int m_ignoreCount = 0;

		bool canClipVelocity = true;
		bool moveAndSlide = true;
		bool debugDraw = true;
		float rotationSpeed = 270.0f;
		float height_offset = 0.0f;

		//void initialize(Entity self, Transform3D& transform, float dt);

		void update(const SceneCamera3D& camera, Entity self, Transform3D& transform, float dt);
		void fixedUpdate(b3WorldId worldId, Entity self, Transform3D& transform, float timestep);
		void render() const;
		void inspect();
		static void Inspect(Entity& e, PlayerMoverController& controller);

		void write(YAML::Emitter& out);
		void read(const YAML::Node& node);
		static void Bind(sol::state& lua);
	private:
		void pollInput(const SceneCamera3D& camera, float dt);
		void SolveMove(b3WorldId worldId, float timeStep, b3Vec3 forward, b3Vec3 right, b3Vec2 throttle, bool clipVelocity);
		void Step(b3WorldId worldId, b3ShapeId* ignoreShapes, int ignoreCount, bool clipVelocity);

		inline b3Vec3 GetVelocity() const {
			return b3Vec3{ velocity.x, velocity.y, velocity.z };
		}

		inline void setVelocity(b3Vec3 p_velocity) {
			velocity = { p_velocity.x, p_velocity.y, p_velocity.z };
		}
	};
}
