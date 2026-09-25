#pragma once
#include "rigidbody3d.hpp"

namespace lib
{
	struct iCollider3D
	{
		enum Collider3DContactFlags
		{
			Collider3DContactFlags_None		= 0,
			Collider3DContactFlags_Contact	= 1 << 0,
			Collider3DContactFlags_Sensor	= 1 << 1,
			Collider3DContactFlags_Hit		= 1 << 2,
			Collider3DContactFlags_Custom	= 1 << 3,

		};

		b3ShapeId id = b3_nullShapeId;
		float density = 1.0f;
		float friction = 0.6f;
		float restitution = 0.0f;
		bool isSensor = false;
		int contactFlags = Collider3DContactFlags_None;

		inline b3Filter getFilter() const
		{
			b3Filter f = b3DefaultFilter();
			//f.categoryBits;

			return f;
		}

		inline b3ShapeDef getDef() const
		{
			b3ShapeDef def = b3DefaultShapeDef();
			def.filter = getFilter();
			def.density = density;
			def.isSensor = isSensor;
			def.baseMaterial.friction = friction;
			def.baseMaterial.restitution = restitution;
			def.enableContactEvents = (contactFlags & Collider3DContactFlags_Contact) != 0;
			def.enableSensorEvents = (contactFlags & Collider3DContactFlags_Sensor) != 0;
			def.enableHitEvents = (contactFlags & Collider3DContactFlags_Hit) != 0;
			def.enableCustomFiltering = (contactFlags & Collider3DContactFlags_Custom) != 0;
			return def;
		}

		inline bool isValid() const { return b3Shape_IsValid(id); }

		inline void free()
		{
			if (isValid()) 
				b3DestroyShape(id, true);
			id = b3_nullShapeId;
		}

		void onWrite(YAML::Emitter& out);
		void onRead(const YAML::Node& node);
		void onInspect();
		static void Bind(sol::state& lua);
	};


	struct BoxCollider3D : public iCollider3D
	{
		bool autoSize = true;
		Vector3 size = { 0.5f, 0.5f, 0.5f };

		inline void init(b3BodyId p_bodyId, const Vector3& p_size, uint32_t p_userdata)
		{
			b3ShapeDef def = getDef();
			def.userData = (void*)(intptr_t)(&p_userdata);

			Vector3 l_size = autoSize ? p_size * 0.5f : size;

			b3BoxHull hull = b3MakeBoxHull(l_size.x, l_size.y, l_size.z);

			id = b3CreateHullShape(p_bodyId, &def, &hull.base);
		}

		void write(YAML::Emitter& out);
		void read(const YAML::Node& node);
		void inspect();
	};
	struct SphereCollider3D : public iCollider3D
	{
		Vector3 origin = { 0.f, 0.f, 0.f };
		float radius = 0.5f;

		inline void init(b3BodyId p_bodyId, const Vector3& p_size, uint32_t p_userdata)
		{
			if (isValid())
				return;

			b3ShapeDef def = getDef();
			def.userData = (void*)(intptr_t)(&p_userdata);

			b3Sphere sphere;
				sphere.center = b3Vec3{ origin.x, origin.y, origin.z };
				sphere.radius = radius;

			id = b3CreateSphereShape(p_bodyId, &def, &sphere);
		}

		void write(YAML::Emitter& out);
		void read(const YAML::Node& node);
		void inspect();
	};

}