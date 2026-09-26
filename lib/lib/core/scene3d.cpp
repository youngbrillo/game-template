#include "lib/core/scene3d.hpp"
#include "lib/components/components3d.hpp"

namespace lib
{
	Scene3D::Scene3D(SceneSettings p_settings)
		: iScene(p_settings)
		, worldId(b3_nullWorldId)
	{
		viewport.init();
	}
	Scene3D::~Scene3D()
	{
		viewport.free();
	}
	void Scene3D::init()
	{
		b3WorldDef wdef = b3DefaultWorldDef();
		worldId = b3CreateWorld(&wdef);

		this->LoadFromFile(settings.configPath);
		this->onInit();
		
		for (auto&& [id, t, rb] : world.view<Transform3D, Rigidbody3D>().each())
		{
			rb.init(worldId, t, (uint32_t)id);
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
			this->onScriptInitalized();
			mainScript.ExecuteScriptFunction("onInit");
		}


	}
	void Scene3D::free()
	{
		this->onFree();
		if (mainScript.isEnabled())
		{
			mainScript.ExecuteScriptFunction("onFree");
		}
		b3DestroyWorld(worldId);
		world.clear();
		mainScript.free();
	}
	void Scene3D::update(float dt)
	{
		this->onUpdate(dt);
	}
	void Scene3D::fixedUpdate(float timestep)
	{
		this->onFixedUpdate(timestep);
		b3World_Step(worldId, timestep, 4);
		world.view<Transform3D, Rigidbody3D>().each(Rigidbody3D::FixedUpdate);
	}
	void Scene3D::render()
	{
		if (viewport.can_draw_to_target)
		{
			viewport.begin();
				this->renderScene();
			viewport.end();

			if (viewport.can_draw_to_screen)
			{
				viewport.render();
			}
		}
		else
			this->renderScene();

	}
	void Scene3D::renderScene()
	{
		const SceneCamera3D& camera_in_use = default_camera;
		BeginMode3D(camera_in_use);
			this->onRender3D(camera_in_use);
		EndMode3D();
		this->onRenderUI();
	}
	void Scene3D::inspect()
	{
	}
	const SceneCamera3D& Scene3D::getSceneCamera() const
	{
		// TODO: insert return statement here
		for (auto&& [id, cam] : world.view<SceneCamera3D>(entt::exclude<components::DisabledTag>).each())
		{
			return cam;
		}

		return default_camera;
	}
}