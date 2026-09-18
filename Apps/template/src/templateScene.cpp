#include "scenes_registration.h"
#include "lib/components/transform3d.hpp"
#include "lib/components/sceneCamera3d.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/utils/random_funcs.hpp"
#include <box3d/box3d.h>

namespace lib
{

	static 	b3Vec3 Vector3_to_b3Vec3(const Vector3& v)
	{
		b3Vec3 r;
		r.x = v.x;
		r.y = v.y;
		r.z = v.z;
		return r;
	}

	static 	Vector3 b3Vec3_to_Vector3(const b3Vec3& v)
	{
		Vector3 r;
		r.x = v.x;
		r.y = v.y;
		r.z = v.z;
		return r;
	}

	static 	b3Quat Quaternion_to_b3Quat(const Quaternion& v)
	{
		b3Quat r = { { v.x, v.y, v.z },v.w };
		return r;
	}

	static 	Quaternion b3Quat_to_Quaternion(const b3Quat& v)
	{
		Quaternion r = { v.v.x, v.v.y, v.v.z, v.s };
		return r;
	}


	struct MeshRefComponent
	{
		int meshId = 0;
		Color tint = RAYWHITE;

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Flow << YAML::BeginMap
				<< YAML::Key << "id" << meshId
				<< YAML::Key << "tint" << tint
				<< YAML::EndMap;
		}

		void Deserialize(const YAML::Node& node)
		{
			bool n = readYamlValue(node["id"], &meshId);
			bool k = readYamlValue(node["tint"], &tint);
		}
	};
	struct RigidBodyComponent
	{
		b3BodyId id = b3_nullBodyId;
		b3BodyType type = b3BodyType::b3_staticBody;
		
		void init(b3WorldId worldId, const Transform3D& t)
		{
			b3BodyDef def = b3DefaultBodyDef();
			def.type = type;
			def.position = Vector3_to_b3Vec3(t.position);
			def.rotation = Quaternion_to_b3Quat(QuaternionNormalize(t.orientation));

			id = b3CreateBody(worldId, &def);
		}

		void Serialize(YAML::Emitter& out)
		{
			int _type = type;

			out << YAML::Flow << YAML::BeginMap
				<< YAML::Key << "type" << _type
				<< YAML::EndMap;
		}

		void Deserialize(const YAML::Node& node)
		{
			int _type = type;
			if (readYamlValue(node["type"], &_type))
			{
				type = (b3BodyType)_type;
			}
			
		}
	};
	struct BoxCollider
	{
		b3ShapeId id = b3_nullShapeId;
		float restitution = 0.4f;
		float friction = 0.4f;

		void init(b3WorldId worldId, b3BodyId bodyId, const Transform3D& t)
		{
			b3ShapeDef def = b3DefaultShapeDef();
			def.baseMaterial.friction = friction;
			def.baseMaterial.restitution = restitution;


			b3BoxHull hull = b3MakeBoxHull(t.size.x * 0.5f, t.size.y * 0.5f, t.size.z * 0.5f);
			id = b3CreateHullShape(bodyId, &def, &hull.base);
		}

		void Serialize(YAML::Emitter& out)
		{
			out << YAML::Flow << YAML::BeginMap
				<< YAML::Key << "restitution" << restitution
				<< YAML::Key << "friction" << friction
				<< YAML::EndMap;
		}

		void Deserialize(const YAML::Node& node)
		{
			readYamlValue(node["restitution"], &restitution);
			readYamlValue(node["friction"], &friction);
		}
	};

	void SerializeAppComponents(YAML::Emitter& out, Entity& e)
	{

		if (auto* c = e.tryGet<MeshRefComponent>())
		{
			out << YAML::Key << "MeshRefComponent" << YAML::Value;
			c->Serialize(out);
		}
		if (auto* c = e.tryGet<RigidBodyComponent>())
		{
			out << YAML::Key << "RigidBodyComponent" << YAML::Value;
			c->Serialize(out);
		}
		if (auto* c = e.tryGet<BoxCollider>())
		{
			out << YAML::Key << "BoxCollider" << YAML::Value;
			c->Serialize(out);
		}
	}


	void DeserializeAppComponents(const YAML::Node& root, Entity& e)
	{
		if (auto node = root["MeshRefComponent"])
		{
			auto& c = e.add<MeshRefComponent>();
			c.Deserialize(node);
		}
		if (auto node = root["RigidBodyComponent"])
		{
			auto& c = e.add<RigidBodyComponent>();
			c.Deserialize(node);
		}
		if (auto node = root["BoxCollider"])
		{
			auto& c = e.add<BoxCollider>();
			c.Deserialize(node);
		}
	}

	struct SphereCollider
	{
		b3ShapeId id = b3_nullShapeId;
		float restitution = 0.4f;
		float friction = 0.4f;

		void init(b3WorldId worldId, b3BodyId bodyId, const Transform3D& t)
		{
			b3ShapeDef def = b3DefaultShapeDef();
			def.baseMaterial.friction = friction;
			def.baseMaterial.restitution = restitution;

			b3Sphere sphere = {
				.center = {0.0f, 0.0f, 0.0f},
				.radius = t.size.x * 0.5f
			};

			id = b3CreateSphereShape(bodyId, &def, &sphere);
		}
	};

	class TemplateScene : public iScene
	{
		Model models[2];
		lib::SceneCamera3D camera;
		b3WorldId worldid;

	public:
		TemplateScene(SceneSettings p_settings)
			:iScene(p_settings)
		{
			models[0] = LoadModelFromMesh(GenMeshCube(1, 1, 1));
			models[1] = LoadModelFromMesh(GenMeshSphere(0.5f, 16, 16));
			camera.camera.position = Vector3{ 50,15,35 };

			SetSerializeEntityCallback(SerializeAppComponents);
			SetDeserializeEntityCallback(DeserializeAppComponents);
		}
		~TemplateScene()
		{
			UnloadModel(models[0]);
			UnloadModel(models[1]);
		}

		virtual void init(){
			b3WorldDef wdef = b3DefaultWorldDef();

			worldid = b3CreateWorld(&wdef);

			this->LoadFromFile(settings.configPath);


			for (auto&& [id, t, rb] : world.view<Transform3D, RigidBodyComponent>().each())
			{
				rb.init(worldid, t);
			}

			for (auto&& [id, t, rb, bc] : world.view<Transform3D, RigidBodyComponent, BoxCollider>().each())
			{
				bc.init(worldid, rb.id, t);
			}
			Transform3D t;
			Vector4 tint;
			{
				t.position = { 0.0f, -0.75f, 0.0f };
				t.size   = { 60.0f, 0.5f, 60.0f };

				Entity e = Entity::Create(world, "ground");
				e.add<Transform3D>(t);
				auto& mc = e.add<MeshRefComponent>();
				mc.meshId = 0;
				mc.tint = DARKGRAY;
				e.disableSerialization();

				auto& rb = e.add<RigidBodyComponent>();
				rb.type = b3_staticBody;
				rb.init(worldid, t);
				auto& bc = e.add<BoxCollider>();
				bc.init(worldid, rb.id, t);
			}
			Vector3 axis = { 0, 1, 0 };
			t.size = { 1,1,1 };
			for (int i = 0; i < 40; i++)
			{
				t.position.x = Random_float(-25.0f, 25.0f);
				t.position.y = Random_float(15.0f, 25.0f);
				t.position.z = Random_float(-25.0f, 25.0f);
				
				tint.x = Random_float(0.0f, 1.0f);
				tint.y = Random_float(0.0f, 1.0f);
				tint.z = Random_float(0.0f, 1.0f);
				tint.w = Random_float(0.5f, 1.0f);

				axis.x = Random_float(-1.0f, 1.0f);
				axis.y = Random_float(-1.0f, 1.0f);
				axis.z = Random_float(-1.0f, 1.0f);

				axis = Vector3Normalize(axis);
				t.RotateAroundAxis(axis, Random_float( -180.0f, 180.0f));

				Entity e = CreateBox(t, ColorFromNormalized(tint));
			}
			for (int i = 0; i < 25; i++)
			{
				t.position.x = Random_float(-25.0f, 25.0f);
				t.position.y = Random_float(15.0f, 25.0f);
				t.position.z = Random_float(-25.0f, 25.0f);

				tint.x = Random_float(0.0f, 1.0f);
				tint.y = Random_float(0.0f, 1.0f);
				tint.z = Random_float(0.0f, 1.0f);
				tint.w = Random_float(0.5f, 1.0f);

				Entity e = CreateSphere(t.position, ColorFromNormalized(tint));
			}
		}
		virtual void free(){

			b3DestroyWorld(worldid);
			world.clear();
		}
		virtual void update(float dt){
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
				UpdateCamera(&camera.camera, CAMERA_THIRD_PERSON);
		}
		virtual void fixedUpdate(float timestep){
			b3World_Step(worldid, timestep, 4);
			auto view = world.view<Transform3D, const RigidBodyComponent>().each();

			for (auto&& [id, transform, body] : view)
			{
				b3Vec3 b_pos = b3Body_GetPosition(body.id);
				b3Quat b_qut = b3Body_GetRotation(body.id);

				transform.position.x = b_pos.x;
				transform.position.y = b_pos.y;
				transform.position.z = b_pos.z;

				transform.orientation.x = b_qut.v.x;
				transform.orientation.y = b_qut.v.y;
				transform.orientation.z = b_qut.v.z;
				transform.orientation.w = b_qut.s;
			}

		}
		virtual void render(){
			BeginMode3D(camera);

			auto view = world.view<const Transform3D, const MeshRefComponent>().each();
			for (auto&& [id, t, m] : view) {
				models[m.meshId].transform = t.toMatrix();
				DrawModel(models[m.meshId], Vector3Zeros, 1.0f, m.tint);
			}
			
			//DrawGrid(50, 10.0f);
			EndMode3D();

			DrawText(TextFormat("Scene: %s", settings.name.c_str()), 10, 10, 20, WHITE);
		}

		Entity CreateBox(Transform3D t, Color tint = WHITE)
		{
			Entity e = Entity::Create(world, "box");
			e.add<Transform3D>(t);
			auto& mc = e.add<MeshRefComponent>();
			mc.meshId = 0;
			mc.tint = tint;
			e.disableSerialization();

			auto& rb = e.add<RigidBodyComponent>();
				rb.type = b3_dynamicBody;
				rb.init(worldid, t);
			auto& bc = e.add<BoxCollider>();
				bc.init(worldid, rb.id, t);
			return e;
		}

		Entity CreateSphere(Vector3 p_position, Color tint = WHITE)
		{
			Entity e = Entity::Create(world, "box");
			auto& t = e.add<Transform3D>(p_position);
			auto& mc = e.add<MeshRefComponent>();
			mc.meshId = 1;
			mc.tint = tint;
			e.disableSerialization();

			auto& rb = e.add<RigidBodyComponent>();
				rb.type = b3_dynamicBody;
				rb.init(worldid, t);

			auto& bc = e.add<SphereCollider>();
				bc.init(worldid, rb.id, t);

			return e;
		}
	protected:
		virtual void onDataSave(YAML::Emitter& root) {
		
		}
		virtual void onDataLoad(const YAML::Node& root) {
		
		}
	};
}





lib::iScene* CreateTemplateScene(lib::SceneSettings p_settings)
{
	return new lib::TemplateScene(p_settings);
}