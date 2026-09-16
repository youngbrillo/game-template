#include "scenes_registration.h"
#include "lib/components/transform3d.hpp"
#include "lib/components/sceneCamera3d.hpp"
#include "lib/utils/random_funcs.hpp"

namespace lib
{
	struct MeshRefComponent
	{
		int meshId = 0;
		Color tint = RAYWHITE;
	};

	class TemplateScene : public iScene
	{
		Model models[2];
		lib::SceneCamera3D camera;

	public:
		TemplateScene(SceneSettings p_settings)
			:iScene(p_settings)
		{
			models[0] = LoadModelFromMesh(GenMeshCube(1, 1, 1));
			models[1] = LoadModelFromMesh(GenMeshSphere(0.5f, 16, 16));
		}
		~TemplateScene()
		{
			UnloadModel(models[0]);
			UnloadModel(models[1]);
		}

		virtual void init(){
			Transform3D t;
			Vector4 tint;
			for (int i = 0; i < 40; i++)
			{
				t.position.x = Random_float(-25.0f, 25.0f);
				t.position.y = Random_float(15.0f, 25.0f);
				t.position.z = Random_float(-25.0f, 25.0f);
				
				tint.x = Random_float(0.0f, 1.0f);
				tint.y = Random_float(0.0f, 1.0f);
				tint.z = Random_float(0.0f, 1.0f);
				tint.w = Random_float(0.0f, 1.0f);

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
				tint.w = Random_float(0.0f, 1.0f);

				Entity e = CreateSphere(t.position, ColorFromNormalized(tint));
			}
		}
		virtual void free(){
			world.clear();
		}
		virtual void update(float dt){
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
				UpdateCamera(&camera.camera, CAMERA_THIRD_PERSON);
		}
		virtual void fixedUpdate(float timestep){
		
		}
		virtual void render(){
			BeginMode3D(camera);

			auto view = world.view<const Transform3D, const MeshRefComponent>().each();
			for (auto&& [id, t, m] : view) {
				models[m.meshId].transform = t.toMatrix();
				DrawModel(models[m.meshId], Vector3Zeros, 1.0f, m.tint);
			}
			
			DrawGrid(50, 10.0f);
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

			return e;
		}

		Entity CreateSphere(Vector3 p_position, Color tint = WHITE)
		{
			Entity e = Entity::Create(world, "box");
			e.add<Transform3D>(p_position);
			auto& mc = e.add<MeshRefComponent>();
			mc.meshId = 1;
			mc.tint = tint;
			e.disableSerialization();
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