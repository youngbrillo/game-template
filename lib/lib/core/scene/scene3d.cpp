#include "lib/core/scene/scene3d.hpp"
#include "lib/components/3d/components3d.hpp"
#include "lib/utils/editor_utils.hpp"
#include <imgui_stdlib.h>
#include "ImGuizmo.h"

namespace lib
{
	Scene3D::Scene3D(SceneSettings p_settings)
		: iAppScene(p_settings)
		, worldId(b3_nullWorldId)
	{
	}
	Scene3D::~Scene3D()
	{
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

		if (!settings.scriptPath.empty() && mainScript.LoadFile(settings.scriptPath))
		{
			this->onScriptInitalized();
			mainScript.ExecuteScriptFunction("onInit");
		}
		else
		{
			mainScript.init();
		}

		for (auto&& [id, script] : world.view<ScriptComponent>().each())
		{
			script.init(Entity(id, world), mainScript.state);
		}


	}
	void Scene3D::free()
	{
		this->onFree();


		for (auto&& [id, script] : world.view<ScriptComponent>().each())
		{
			script.free();
		}

		if (mainScript.isEnabled())
		{
			mainScript.ExecuteScriptFunction("onFree");
		}
		b3DestroyWorld(worldId);
		world.clear();
		mainScript.free();
		m_scene_inspector.selected = Entity();
	}
	void Scene3D::update(float dt)
	{
		auto deleted = world.view<components::DeleteTag>();
		world.destroy(deleted.begin(), deleted.end());


		for (auto&& [id, script] : world.view<ScriptComponent>().each())
		{
			script.update(dt);
		}

		world.view<SceneCamera3D, CameraController3D>().each(CameraController3D::Update);

		auto& active_camera = this->getSceneCamera();

		for (auto&& [id, transform, controller] : world.view<Transform3D, PlayerMoverController>().each())
		{
			Entity e(id, world);
			controller.update(active_camera, e, transform, dt);
		}
		this->onUpdate(dt);
	}
	void Scene3D::fixedUpdate(float timestep)
	{
		this->onFixedUpdate(timestep);
		b3World_Step(worldId, timestep, 4);
		world.view<Transform3D, Rigidbody3D>().each(Rigidbody3D::FixedUpdate);

		for (auto&& [id, transform, controller] : world.view<Transform3D, PlayerMoverController>().each())
		{
			Entity e(id, world);
			controller.fixedUpdate(worldId, e, transform, timestep);
		}
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
		const SceneCamera3D& camera_in_use = getSceneCamera();
		BeginMode3D(camera_in_use);
			this->onRender3D(camera_in_use);

			for (auto&& [id, transform, controller] : world.view<Transform3D, PlayerMoverController>().each())
			{
				Entity e(id, world);
				controller.render();
			}
		EndMode3D();
		this->onRenderUI();
	}

	const SceneCamera3D& Scene3D::getSceneCamera() const
	{
		for (auto&& [id, cam] : world.view<SceneCamera3D>(entt::exclude<components::DisabledTag>).each())
		{
			return cam;
		}

		return default_camera;
	}

	void Scene3D::onInspectViewportTop(Entity& selected)
	{

	}
	void Scene3D::onInspectViewportBottom(float* p_viewportPos, float* p_viewportSize, Entity& selected)
	{
		ImVec2 viewportPos; 
			viewportPos.x = p_viewportPos[0];
			viewportPos.y = p_viewportPos[1];
		ImVec2 viewportSize;
			viewportSize.x = p_viewportSize[0];
			viewportSize.y = p_viewportSize[1];

		if (selected)
		{
			if (auto* transform = selected.tryGet<Transform3D>())
			{
				Matrix selected_matrix = transform->toMatrix();
				if (EditorDrawTransformWidget(viewportPos, viewportSize, getSceneCamera(), selected_matrix))
				{
					*transform = selected_matrix;
					if (auto* rb = selected.tryGet<Rigidbody3D>())
					{
						rb->setTransform(*transform);
					}
				}
			}
		}
	}
}