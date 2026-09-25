#include "scenes_registration.h"
#include "lib/components/transform3d.hpp"
#include "lib/components/sceneCamera3d.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/utils/random_funcs.hpp"
#include "lib/scripting/luaScript.hpp"
#include "lib/components/components3d.hpp"

namespace lib
{
	void SerializeAppComponents(YAML::Emitter& out, Entity& e)
	{

		//if (auto* c = e.tryGet<MeshRefComponent>())
		//{
		//	out << YAML::Key << "MeshRefComponent" << YAML::Value;
		//	c->Serialize(out);
		//}
		//if (auto* c = e.tryGet<RigidBodyComponent>())
		//{
		//	out << YAML::Key << "RigidBodyComponent" << YAML::Value;
		//	c->Serialize(out);
		//}
		//if (auto* c = e.tryGet<BoxCollider>())
		//{
		//	out << YAML::Key << "BoxCollider" << YAML::Value;
		//	c->Serialize(out);
		//}
	}

	void DeserializeAppComponents(const YAML::Node& root, Entity& e)
	{
		if (auto node = root["MeshRefComponent"])
		{
			auto& c = e.add<StaticMesh>();
			c.read(node);
		}
		if (auto node = root["RigidBodyComponent"])
		{
			auto& c = e.add<Rigidbody3D>();
			c.read(node);
		}
		if (auto node = root["BoxCollider"])
		{
			auto& c = e.add<BoxCollider3D>();
			c.read(node);
		}
		if (auto node = root["SphereCollider"])
		{
			auto& c = e.add<SphereCollider3D>();
			c.read(node);
		}
	}

	class TemplateScene : public iScene
	{
		Model models[2];
		lib::SceneCamera3D camera;
		b3WorldId worldid;
		LuaScript mainScript;

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

			for (auto&& [id, t, rb] : world.view<Transform3D, Rigidbody3D>().each())
			{
				rb.init(worldid, t, (uint32_t)id);
			}

			for (auto&& [id, t, rb, bc] : world.view<Transform3D, Rigidbody3D, BoxCollider3D>().each())
			{
				bc.init(rb.id, t.size, (uint32_t)id);
			}

			for (auto&& [id, t, rb, bc] : world.view<Transform3D, Rigidbody3D, SphereCollider3D>().each())
			{
				bc.init(rb.id, t.size, (uint32_t)id);
			}

			if (mainScript.LoadFile(settings.scriptPath))
			{
				mainScript.state["CreateBox"] = [=](Transform3D t, Color c) {CreateBox(t, c); };
				mainScript.state["CreateSphere"] = [=](Vector3 v, Color c) {CreateSphere(v, c); };

				mainScript.ExecuteScriptFunction("onInit");
			}
		}
		virtual void free(){

			if (mainScript.isEnabled())
			{
				mainScript.ExecuteScriptFunction("onFree");
			}
			b3DestroyWorld(worldid);
			world.clear();
			mainScript.free();
		}
		virtual void update(float dt){
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
				UpdateCamera(&camera.camera, CAMERA_THIRD_PERSON);
		}
		virtual void fixedUpdate(float timestep){

			b3World_Step(worldid, timestep, 4);
			world.view<Transform3D, Rigidbody3D>().each(Rigidbody3D::FixedUpdate);

		}
		virtual void render(){
			BeginMode3D(camera);

			auto view = world.view<const Transform3D, const StaticMesh>().each();
			for (auto&& [id, t, m] : view) {
				models[m.id].transform = t.toMatrix();
				DrawModel(models[m.id], Vector3Zeros, 1.0f, m.tint);
			}
			
			EndMode3D();

			DrawText(TextFormat("Scene: %s", settings.name.c_str()), 10, 10, 20, WHITE);
		}

		Entity CreateBox(Transform3D t, Color tint = WHITE)
		{
			Entity e = Entity::Create(world, "box");
			e.add<Transform3D>(t);
			auto& mc = e.add<StaticMesh>();
			mc.id= 0;
			mc.tint = tint;
			e.disableSerialization();

			auto& rb = e.add<Rigidbody3D>();
				rb.type = b3_dynamicBody;
				rb.init(worldid, t, e);
			auto& bc = e.add<BoxCollider3D>();
				bc.init(rb.id, t.size, e);
			return e;
		}

		Entity CreateSphere(Vector3 p_position, Color tint = WHITE)
		{
			Entity e = Entity::Create(world, "box");
			auto& t = e.add<Transform3D>(p_position);
			auto& mc = e.add<StaticMesh>();
			mc.id = 1;
			mc.tint = tint;
			e.disableSerialization();

			auto& rb = e.add<Rigidbody3D>();
				rb.type = b3_dynamicBody;
				rb.init(worldid, t, e);

			auto& bc = e.add<SphereCollider3D>();
				bc.init(rb.id, t.size, e);

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