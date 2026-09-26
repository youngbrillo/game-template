#include "playerMover.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include <imgui.h>
#include "lib/core/input_manager.hpp"

namespace lib
{
	static Color b3_purple = GetColor(0x800080FF);
	static Color b3_yellow = GetColor(0xFFFF00FF);

	static bool MoverFilterCallback(b3ShapeId shapeId, void* context)
	{
		PlayerMoverController* self = (PlayerMoverController*)context;
		for (int i = 0; i < self->m_ignoreCount; ++i)
		{
			if (B3_ID_EQUALS(shapeId, self->m_ignoreShapeIds[i]))
			{
				return false;
			}
		}
		return true;
	}

	static bool PlaneResultFcn(b3ShapeId shapeId, const b3PlaneResult* planeResults, int planeCount, void* context)
	{
		if (MoverFilterCallback(shapeId, context) == false){
			// ignore these planes but continue looking for more
			return true;
		}
		PlayerMoverController* self = static_cast<PlayerMoverController*>(context);
		float maxPush = FLT_MAX;
		bool clipVelocity = true;
		PlayerMoverController* userData = nullptr;
		if (userData != nullptr)
		{
			//maxPush = userData->maxPush;
			//clipVelocity = userData->clipVelocity;
		}

		for (int i = 0; i < planeCount && self->m_planeCount < PlayerMoverController::m_planeCapacity; ++i)
		{
			assert(b3IsValidPlane(planeResults[i].plane));
			self->m_planes[self->m_planeCount] = {
				.plane = planeResults[i].plane,
				.pushLimit = maxPush,
				.push = 0.0f,
				.clipVelocity = clipVelocity,
			};
			self->m_planeExtras[self->m_planeCount] = {
				.point = b3OffsetPos(self->m_transform.p, planeResults[i].point),
				.shapeId = shapeId,
			};
			self->m_planeCount += 1;
		}

		return true;
	}

	void PlayerMoverController::update(const SceneCamera3D& camera, Entity self, Transform3D& transform, float dt)
	{
		m_transform.p = { transform.position.x, transform.position.y + height_offset, transform.position.z };
		pollInput(camera, dt);

		if (direction.x != 0 && direction.y != 0)
		{
			Vector2 normalDir = Vector2Normalize(direction);
			transform.orientation = transform.RotateTowards(Vector3{ normalDir.x, 0, normalDir.y }, Vector3{ 0, 1, 0 }, rotationSpeed * dt);
		}
	}
	void PlayerMoverController::fixedUpdate(b3WorldId worldId, Entity self, Transform3D& transform, float timestep)
	{
		Step(worldId, nullptr, 0, canClipVelocity);
		transform.position = { m_transform.p.x, m_transform.p.y- height_offset, m_transform.p.z };
	}
	void PlayerMoverController::render() const
	{
		if (!debugDraw)
			return;

		b3Pos position = m_transform.p;
		int count = m_planeCount;
		for (int i = 0; i < count; ++i)
		{
			b3Plane plane = m_planes[i].plane;
			b3Pos p1 = position + (plane.offset - m_capsule.radius) * plane.normal;
			b3Pos p2 = p1 + 0.1f * plane.normal;

			DrawSphereWires(Vector3{ p1.x, p1.y, p1.z }, 0.15f, 16, 16, b3_yellow);
			DrawLine3D(Vector3{ p1.x, p1.y, p1.z }, Vector3{ p2.x, p2.y, p2.z }, b3_yellow);
		}

		b3Vec3 a = m_transform.p + m_capsule.center1;
		b3Vec3 b = m_transform.p + m_capsule.center2;

		Vector3 capsule_start = Vector3{ a.x, a.y, a.z };
		Vector3 capsule_end = Vector3{ b.x, b.y, b.z };
		float capsule_rad = m_capsule.radius;
		int capsule_rings = 8;
		int capsule_slices = 16;
		DrawCapsuleWires(capsule_start, capsule_end, capsule_rad, capsule_rings, capsule_slices, b3_purple);
		b3Vec3 pv = position + GetVelocity();
		DrawLine3D(Vector3{ position.x,position.y, position.z }, Vector3{ pv.x, pv.y, pv.z }, b3_purple);
	}
	void PlayerMoverController::inspect()
	{
		ImGui::Checkbox("on ground", &m_onGround);
		ImGui::Checkbox("m_sprint", &m_sprint);
		ImGui::Checkbox("Can Clip Velocity", &canClipVelocity);
		ImGui::Checkbox("Move and Slide", &moveAndSlide);
		ImGui::Checkbox("Debug Draw", &debugDraw);

		ImGui::DragFloat2("direction", &direction.x, 0.1f);
		ImGui::DragFloat3("velocity", &velocity.x, 0.1f);

		ImGui::DragFloat("movementSpeed", &movementSpeed, 0.1f);
		ImGui::DragFloat("jumpSpeed", &jumpSpeed, 0.1f);
		ImGui::DragFloat("maxSpeed", &maxSpeed, 0.1f);
		ImGui::DragFloat("minSpeed", &minSpeed, 0.1f);
		ImGui::DragFloat("stopSpeed", &stopSpeed, 0.1f);
		ImGui::DragFloat("accelerate", &accelerate, 0.1f);
		ImGui::DragFloat("friction", &friction, 0.1f);
		ImGui::DragFloat("gravity", &gravity, 0.1f);
		ImGui::DragFloat("sprintMultiplier", &sprintMultiplier, 0.1f);
		ImGui::SliderFloat("rotationSpeed", &rotationSpeed, 0.0f, 540.0f);
		ImGui::SliderFloat("height offset", &height_offset, -10.0f, 10.0f);

		if (ImGui::TreeNode("capsule"))
		{
			ImGui::DragFloat("radius", &m_capsule.radius, 0.1f);
			ImGui::DragFloat3("center1", &m_capsule.center1.x, 0.1f);
			ImGui::DragFloat3("center2", &m_capsule.center2.x, 0.1f);

			ImGui::TreePop();
		}

		ImGui::InputInt("m_planeCount", &m_planeCount);
		ImGui::InputInt("m_totalIterations", &m_totalIterations);

		ImGui::DragFloat("m_pogoVelocity", &m_pogoVelocity, 0.1f);
		ImGui::SliderFloat("pogoLength", &pogoLength, 0.1f, 10);
	}
	void PlayerMoverController::Inspect(Entity& e, PlayerMoverController& controller)
	{
		controller.inspect();
	}
	void PlayerMoverController::write(YAML::Emitter& out)
	{
		Vector3 center1 = { m_capsule.center1.x, m_capsule.center1.y, m_capsule.center1.z };
		Vector3 center2 = { m_capsule.center2.x, m_capsule.center2.y, m_capsule.center2.z };
		out
			//<< YAML::Flow		//turn off flow, because there will only-really-ever be 1 of these
			<< YAML::BeginMap
			<< YAML::Key << "movementSpeed" << movementSpeed
			<< YAML::Key << "jumpSpeed" << jumpSpeed
			<< YAML::Key << "maxSpeed" << maxSpeed
			<< YAML::Key << "minSpeed" << minSpeed
			<< YAML::Key << "stopSpeed" << stopSpeed
			<< YAML::Key << "accelerate" << accelerate
			<< YAML::Key << "friction" << friction
			<< YAML::Key << "gravity" << gravity
			<< YAML::Key << "sprintMultiplier" << sprintMultiplier
			<< YAML::Key << "canClipVelocity" << canClipVelocity
			<< YAML::Key << "moveAndSlide" << moveAndSlide
			<< YAML::Key << "debugDraw" << debugDraw
			<< YAML::Key << "height_offset" << height_offset
			<< YAML::Key << "capsule"
			<< YAML::Flow << YAML::BeginMap
			<< YAML::Key << "radius" << m_capsule.radius
			<< YAML::Key << "center1" << center1
			<< YAML::Key << "center2" << center2
			<< YAML::EndMap
			<< YAML::EndMap
			;
	}
	void PlayerMoverController::read(const YAML::Node& node)
	{
		readYamlValue(node["movementSpeed"], &movementSpeed);
		readYamlValue(node["jumpSpeed"], &jumpSpeed);
		readYamlValue(node["maxSpeed"], &this->maxSpeed);
		readYamlValue(node["minSpeed"], &this->minSpeed);
		readYamlValue(node["stopSpeed"], &this->stopSpeed);
		readYamlValue(node["accelerate"], &this->accelerate);
		readYamlValue(node["friction"], &this->friction);
		readYamlValue(node["gravity"], &this->gravity);
		readYamlValue(node["sprintMultiplier"], &this->sprintMultiplier);
		readYamlValue(node["canClipVelocity"], &this->canClipVelocity);
		readYamlValue(node["moveAndSlide"], &this->moveAndSlide);
		readYamlValue(node["debugDraw"], &this->debugDraw);
		readYamlValue(node["height_offset"], &this->height_offset);
		if (auto c = node["capsule"])
		{
			Vector3 center1 = { this->m_capsule.center1.x, this->m_capsule.center1.y, this->m_capsule.center1.z };
			Vector3 center2 = { this->m_capsule.center2.x, this->m_capsule.center2.y, this->m_capsule.center2.z };
			readYamlValue(c["radius"], &this->m_capsule.radius);
			if (readYamlValue(c["center1"], &center1))
			{
				this->m_capsule.center1 = { center1.x,center1.y,center1.z };
			}
			if (readYamlValue(c["center2"], &center2))
			{
				this->m_capsule.center2 = { center2.x,center2.y,center2.z };
			}
		}
	}
	void PlayerMoverController::Bind(sol::state& lua)
	{
	}
	void PlayerMoverController::pollInput(const SceneCamera3D& camera, float dt)
	{
		direction = InputManager::GetMoveDirection();
		direction = camera.GetRelativeDirection(direction);
		m_sprint = InputManager::isActionDown(ActionSprint);

		if (InputManager::isActionDown(ActionConfirm) && m_onGround)
		{
			velocity.y = jumpSpeed;
			m_onGround = false;
		}

	}
	void PlayerMoverController::SolveMove(b3WorldId worldId, float timeStep, 
		b3Vec3 forward, b3Vec3 right, b3Vec2 throttle, bool clipVelocity)
	{
		b3Vec3 m_velocity = GetVelocity();

		// Friction
		float speed = b3Length(m_velocity);
		if (speed < minSpeed)
		{
			m_velocity.x = 0.0f;
			m_velocity.z = 0.0f;
		}
		else
		{
			// Linear damping above stopSpeed and fixed reduction below stopSpeed
			float control = speed < stopSpeed ? stopSpeed : speed;

			// friction has units of 1/time
			float drop = control * friction * timeStep;
			float newSpeed = b3MaxFloat(0.0f, speed - drop);
			float ratio = newSpeed / speed;
			m_velocity.x *= ratio;
			m_velocity.z *= ratio;
		}

		float max_speed = m_sprint ? 1.5f * maxSpeed : this->maxSpeed;

		//b3Vec3 desiredVelocity = max_speed * throttle.x * forward + max_speed * throttle.y * right;
		b3Vec3 desiredVelocity = b3Vec3{ direction.x, 0, direction.y } *max_speed;
		float desiredSpeed;
		b3Vec3 desiredDirection = b3GetLengthAndNormalize(&desiredSpeed, desiredVelocity);

		if (desiredSpeed > max_speed)
		{
			desiredVelocity *= max_speed / desiredSpeed;
			desiredSpeed = max_speed;
		}

		if (m_onGround)
		{
			m_velocity.y = 0.0f;
		}

		// Accelerate
		float currentSpeed = b3Dot(m_velocity, desiredDirection);
		float addSpeed = desiredSpeed - currentSpeed;
		if (addSpeed > 0.0f)
		{
			float accelSpeed = accelerate * max_speed * timeStep;
			if (accelSpeed > addSpeed)
			{
				accelSpeed = addSpeed;
			}

			m_velocity += accelSpeed * desiredDirection;
		}

		m_velocity.y -= gravity * timeStep;

		float pogoRestLength = pogoLength * m_capsule.radius;
		float rayLength = pogoRestLength + m_capsule.radius;
		b3Pos rayOrigin = b3TransformWorldPoint(m_transform, m_capsule.center1);
		b3Vec3 rayTranslation = -rayLength * b3Vec3_axisY;
		b3QueryFilter skipTeamFilter = { 1, ~2u };
		skipTeamFilter.name = "pogo";
		b3RayResult rayResult = b3World_CastRayClosest(worldId, rayOrigin, rayTranslation, skipTeamFilter);

		// After gravity was applied, disable pogo when still moving up.
		// Avoids getting pulled back to the ground when jumping.
		bool suppressPogo = m_velocity.y > 0.0f;

		if (rayResult.hit == false || suppressPogo)
		{
			m_onGround = false;
			m_pogoVelocity = 0.0f;

			//DrawLine(rayOrigin, b3OffsetPos(rayOrigin, rayTranslation), MakeColor(b3_colorGray));
		}
		else
		{
			m_onGround = true;
			float pogoCurrentLength = rayResult.fraction * rayLength;

			float zeta = 0.7f;
			float hertz = 4.0f;
			float omega = 2.0f * B3_PI * hertz;
			float omegaH = omega * timeStep;

			m_pogoVelocity = (m_pogoVelocity - omega * omegaH * (pogoCurrentLength - pogoRestLength)) /
				(1.0f + 2.0f * zeta * omegaH + omegaH * omegaH);
			//DrawLine(rayOrigin, rayResult.point, MakeColor(b3_colorGreen));
		}

		b3Pos startPosition = m_transform.p;
		b3Pos target = m_transform.p + timeStep * m_velocity + timeStep * m_pogoVelocity * b3Vec3_axisY;

		// Want the mover to collide with allies
		b3QueryFilter moverFilter = { .categoryBits = 1, .maskBits = ~0u, .id = 1, .name = "mover_collide" };

		// The cast should ignore allies
		b3QueryFilter castFilter = { .categoryBits = 1, .maskBits = ~2u, .id = 1, .name = "mover_cast" };

		m_totalIterations = 0;
		float tolerance = 0.01f;

		for (int iteration = 0; iteration < 5; ++iteration)
		{
			m_planeCount = 0;

			b3Capsule mover;
			mover.center1 = m_capsule.center1;
			mover.center2 = m_capsule.center2;
			mover.radius = m_capsule.radius;

			b3World_CollideMover(worldId, m_transform.p, &mover, moverFilter, PlaneResultFcn, this);

			b3Vec3 targetDelta = target - m_transform.p;
			b3PlaneSolverResult result = b3SolvePlanes(targetDelta, m_planes, m_planeCount);

			m_totalIterations += result.iterationCount;

			b3Vec3 delta = result.delta;

			float fraction = b3World_CastMover(worldId, m_transform.p, &mover, delta, castFilter, MoverFilterCallback, this);

			delta *= fraction;
			m_transform.p = m_transform.p + delta;

			if (b3LengthSquared(delta) < tolerance * tolerance)
			{
				break;
			}
		}

		for (int i = 0; i < m_planeCount; ++i)
		{
			b3BodyId bodyId = b3Shape_GetBody(m_planeExtras[i].shapeId);
			b3BodyType bodyType = b3Body_GetType(bodyId);
			if (bodyType != b3_dynamicBody)
			{
				continue;
			}

			b3Pos point = m_planeExtras[i].point;
			b3Vec3 normal = b3Neg(m_planes[i].plane.normal);

			float invMassA = 0.0f;
			float invMassB = b3Body_GetInverseMass(bodyId);
			b3Matrix3 invIB = b3Body_GetWorldInverseRotationalInertia(bodyId);

			b3Pos pB = b3Body_GetWorldCenter(bodyId);
			b3Vec3 rB = b3SubPos(point, pB);

			b3Vec3 rnB = b3Cross(rB, normal);
			float kNormal = invMassA + invMassB + b3Dot(rnB, b3MulMV(invIB, rnB));
			float normalMass = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;

			b3Vec3 vB = b3Body_GetLinearVelocity(bodyId);
			b3Vec3 omegaB = b3Body_GetAngularVelocity(bodyId);
			b3Vec3 vrB = b3Add(vB, b3Cross(omegaB, rB));
			float vn = b3Dot(b3Sub(vrB, m_velocity), normal);
			float impulse = b3MaxFloat(-normalMass * vn, 0.0f);

			b3Vec3 P = b3MulSV(impulse, normal);
			m_velocity = b3MulSub(m_velocity, invMassA, P);

			b3Body_ApplyLinearImpulse(bodyId, P, point, true);
		}

		if (clipVelocity)
		{
			// Using the velocity clipper can avoid picking up velocity from depenetration.
			// This allows the mover to avoid velocity from soft collision depenetration.
			if (moveAndSlide)
				m_velocity = b3ClipVector(m_velocity, m_planes, m_planeCount);
		}
		else if (timeStep > 0.0f)
		{
			// Using the position delta is more holistic and intuitive in some cases.
			m_velocity = (1.0f / timeStep) * (m_transform.p - startPosition);
		}

		setVelocity(m_velocity);
	}
	void PlayerMoverController::Step(b3WorldId worldId, b3ShapeId* ignoreShapes, int ignoreCount, bool clipVelocity)
	{
		m_ignoreShapeIds = ignoreShapes;
		m_ignoreCount = ignoreCount;

		float hertz = 60.0f;
		float timeStep = hertz > 0.0f ? 1.0f / hertz : 0.0f;

		// throttle = { 0.0f, 0.0f, -1.0f };

		SolveMove(worldId, timeStep, b3Vec3{ 0 }, b3Vec3{ 0 }, b3Vec2{ 0 }, clipVelocity);
	}
}