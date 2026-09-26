#include "scenes_registration.h"
#include "lib/core/scene3d.hpp"
#include "lib/components/components3d.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/utils/random_funcs.hpp"
#include "lib/scripting/luaScript.hpp"

namespace lib
{
	void SerializeAppComponents(YAML::Emitter& out, Entity& e)
	{

	}

	void DeserializeAppComponents(const YAML::Node& root, Entity& e)
	{

	}

	class TemplateScene : public Scene3D
	{
		Model models[2];
	public:
		TemplateScene(SceneSettings p_settings)
			:Scene3D(p_settings)
		{
			models[0] = LoadModelFromMesh(GenMeshCube(1, 1, 1));
			models[1] = LoadModelFromMesh(GenMeshSphere(0.5f, 16, 16));
			default_camera.camera.position = Vector3{ 50,15,35 };

			SetSerializeEntityCallback(SerializeAppComponents);
			SetDeserializeEntityCallback(DeserializeAppComponents);
		}
		~TemplateScene()
		{
			UnloadModel(models[0]);
			UnloadModel(models[1]);
		}

		virtual void onScriptInitalized() override {
			mainScript.state["CreateEntity"] = [=](const std::string& name) { return Entity::Create(world, name); };
			mainScript.state["CreateBox"] = [=](Transform3D t, Color c) {return CreateBox(t, c); };
			mainScript.state["CreateSphere"] = [=](Vector3 v, Color c) {return CreateSphere(v, c); };
		}
		virtual void onFree() override {

		}
		virtual void onUpdate(float dt) override{
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
				UpdateCamera(&default_camera.camera, CAMERA_THIRD_PERSON);
		}
		virtual void onFixedUpdate(float timestep) override{


		}
		virtual void onRender3D(const SceneCamera3D& camera) override{
			auto view = world.view<const Transform3D, const StaticMesh>().each();
			for (auto&& [id, t, m] : view) {
				models[m.id].transform = t.toMatrix();
				DrawModel(models[m.id], Vector3Zeros, 1.0f, m.tint);
			}

		}
		virtual void onRenderUI() override {
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
				rb.init(worldId, t, e);
			auto& bc = e.add<BoxCollider3D>();
				bc.init(rb.id, t.size, e);
			return e;
		}

		Entity CreateSphere(Vector3 p_position, Color tint = WHITE)
		{
			Entity e = Entity::Create(world, "sphere");
			auto& t = e.add<Transform3D>(p_position);
			auto& mc = e.add<StaticMesh>();
			mc.id = 1;
			mc.tint = tint;
			e.disableSerialization();

			auto& rb = e.add<Rigidbody3D>();
				rb.type = b3_dynamicBody;
				rb.init(worldId, t, e);

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





lib::Scene3D* CreateTemplateScene(lib::SceneSettings p_settings)
{
	return new lib::TemplateScene(p_settings);
}