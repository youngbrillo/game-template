#include "lib/core/scene3d.hpp"
#include "lib/components/components3d.hpp"
#include "lib/utils/editor_utils.hpp"
#include <imgui_stdlib.h>
#include "lib/utils/ImGuizmo/ImGuizmo.h"

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
		m_scene_inspector.selected = Entity();
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
		this->inspectSettings(m_scene_inspector.selected);
		this->inspectEntity(m_scene_inspector.selected);
		this->inspectViewport(m_scene_inspector.selected);
		this->onInspectWindow();

		if (this->m_scene_inspector.window.demo)
			ImGui::ShowDemoWindow(&m_scene_inspector.window.demo);
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
	void Scene3D::inspectSettings(Entity& selected)
	{
		if (!m_scene_inspector.window.settings)
			return;

		ImGui::Begin("Scene Details", &m_scene_inspector.window.settings);
			ImGui::InputText("Name", &settings.name);
			ImGui::InputText("Config", &settings.configPath);
			ImGui::InputText("Script", &settings.scriptPath);


			ImGui::SeparatorText("world");

			EditorInspectSceneEntities(world, m_scene_inspector.selected);

		ImGui::End();
	}
	void Scene3D::inspectEntity(Entity& selected)
	{
		if (!selected)
			return;

		if (!m_scene_inspector.window.entity)
			return;

		ImGui::Begin("Entity Details", &m_scene_inspector.window.entity);

		selected.inspect();

		ImGui::End();
	}
	void Scene3D::inspectViewport(Entity& selected)
	{
		if (!m_scene_inspector.window.viewport)
			return;

		// If ImGuizmo is hovered or active, block ImGui from dragging the window!
		int windowFlags = 0;
		// Copy modified transform back to our Raylib matrix if interacted
		if (ImGuizmo::IsOver())
			windowFlags |= ImGuiWindowFlags_NoMove;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_FirstUseEver);
		ImGui::Begin("Viewport", &m_scene_inspector.window.viewport, windowFlags);

		// Get exact screen space bounds of the ImGui viewport tab
		ImVec2 viewportPos = ImGui::GetCursorScreenPos();
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		if (viewportSize.x < 1.0f) viewportSize.x = 1.0f;
		if (viewportSize.y < 1.0f) viewportSize.y = 1.0f;

		ImVec2 imgSize;
		ImVec2 imageScreenPos;
		float xPadding = 0;
		float yPadding = 0;
		//position the image in the viewport
		float imageAspectRatio = (float)viewport.width / (float)viewport.height;
		float viewportAspectRatio = viewportSize.x / viewportSize.y;

		if (viewportAspectRatio > imageAspectRatio)
		{
			imgSize.x = viewportSize.y * imageAspectRatio;
			imgSize.y = viewportSize.y;
		}
		else
		{
			imgSize.x = viewportSize.x;
			imgSize.y = viewportSize.x / imageAspectRatio;
		}

		xPadding = (viewportSize.x - imgSize.x) / 2.0f;
		yPadding = (viewportSize.y - imgSize.y) / 2.0f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xPadding);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yPadding);
		viewportPos = ImGui::GetCursorScreenPos();
		viewportSize = imgSize;
		// Render Scene Texture (Flipped Y for OpenGL UV space)
		ImTextureID texID = (ImTextureID)(intptr_t)viewport.target.texture.id;
		ImGui::Image(texID, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

		if (selected)
		{
			if (auto* transform = selected.tryGet<Transform3D>())
			{
				Matrix selected_matrix = transform->toMatrix();
				if (EditorDrawTransformWidget(viewportPos, viewportSize, default_camera, selected_matrix))
				{
					*transform = selected_matrix;
					if (auto* rb = selected.tryGet<Rigidbody3D>())
					{
						rb->setTransform(*transform);
					}
				}
			}
		}


		ImGui::End();
		ImGui::PopStyleVar();
	}
}