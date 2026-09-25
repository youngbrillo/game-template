#pragma once
#include "lib/components/transform3d.hpp"
#include "lib/utils/math_utils.hpp"
#include "box3d/box3d.h"
namespace lib
{
	struct Rigidbody3D
	{
		enum RigidBody3DMotionLocks
		{
			RigidBody3DMotionLock_none = 0,
			RigidBody3DMotionLock_linearX = 1 << 0,
			RigidBody3DMotionLock_linearY = 1 << 1,
			RigidBody3DMotionLock_linearZ = 1 << 2,
			RigidBody3DMotionLock_angularX = 1 << 3,
			RigidBody3DMotionLock_angularY = 1 << 4,
			RigidBody3DMotionLock_angularZ = 1 << 5,
		};

		b3BodyId id = b3_nullBodyId;
		int type = b3BodyType::b3_staticBody;
		int motionLocks = RigidBody3DMotionLock_none;
		float gravityScale = 1.0f;

		inline b3MotionLocks getMotionLocks() const
		{
			b3MotionLocks locks;
			locks.linearX	= (motionLocks & RigidBody3DMotionLock_linearX) != 0;
			locks.linearY	= (motionLocks & RigidBody3DMotionLock_linearY) != 0;
			locks.linearZ	= (motionLocks & RigidBody3DMotionLock_linearZ) != 0;
			locks.angularX	= (motionLocks & RigidBody3DMotionLock_angularX) != 0;
			locks.angularY	= (motionLocks & RigidBody3DMotionLock_angularY) != 0;
			locks.angularZ	= (motionLocks & RigidBody3DMotionLock_angularZ) != 0;
			return locks;
		}

		inline b3BodyType getType() const
		{
			b3BodyType _type = b3BodyType::b3_staticBody;

			switch (type)
			{
			case b3_staticBody:
				_type = b3_staticBody;
				break;
			case b3_kinematicBody:
				_type = b3_kinematicBody;
				break;
			case b3_dynamicBody:
			default:
				_type = b3_dynamicBody;
				break;
			}

			return _type;
		}

		inline b3BodyDef getDef() const
		{
			b3BodyDef def = b3DefaultBodyDef();
			def.type = getType();
			def.motionLocks = getMotionLocks();
			def.gravityScale = gravityScale;
			return def;
		}

		inline bool isValid() const { return b3Body_IsValid(id);}

		inline void init(b3WorldId p_worldId, const Transform3D& p_transform, uint32_t p_userdata)
		{
			b3BodyDef def = getDef();
			float3_copy(&p_transform.position.x, &def.position.x);
			float4_copy(&p_transform.orientation.x, &def.rotation.v.x);
			def.userData = (void*)(intptr_t)(&p_userdata);

			id = b3CreateBody(p_worldId, &def);
		}
		inline void free()
		{
			if (isValid()) b3DestroyBody(id);
			id = b3_nullBodyId;
		}

		inline void setTransform(const Transform3D& p_transform)
		{
			if (isValid())
			{
				b3Vec3 pos;
				b3Quat rot;

				float3_copy(&p_transform.position.x, &pos.x);
				float4_copy(&p_transform.orientation.x, &rot.v.x);
				b3Body_SetTransform(id, pos, rot);
			}
		}

		inline void applyTransform(Transform3D& p_transform) const
		{
			if (isValid())
			{
				auto t = b3Body_GetTransform(id);
				float3_copy(&t.p.x, &p_transform.position.x);
				float4_copy(&t.q.v.x, &p_transform.orientation.x);
			}
		}

		void write(YAML::Emitter& out);
		void read(const YAML::Node& node);
		void inspect();
		static void Bind(sol::state& lua);
		static void Inspect(Entity e, Rigidbody3D& component);
		static void FixedUpdate(Transform3D& p_transform, Rigidbody3D& p_rigidbody);
	};

}
