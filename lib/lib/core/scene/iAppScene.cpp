#include "iAppScene.hpp"
#include "lib/utils/editor_utils.hpp"
#include <imgui_stdlib.h>
#include "ImGuizmo.h"

namespace lib
{
	iAppScene::iAppScene(SceneSettings p_settings)
		: iScene(p_settings)
	{
		viewport.init();
	}
	iAppScene::~iAppScene()
	{
		viewport.free();
	}

	void iAppScene::render()
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

	void iAppScene::inspect()
	{
		this->inspectSettings(m_scene_inspector.selected);
		this->inspectEntity(m_scene_inspector.selected);
		this->inspectViewport(m_scene_inspector.selected);
		this->onInspectWindow();

		if (this->m_scene_inspector.window.demo)
			ImGui::ShowDemoWindow(&m_scene_inspector.window.demo);
	}
	void iAppScene::inspectSettings(Entity& selected)
	{
		if (!m_scene_inspector.window.settings)
			return;

		ImGui::Begin("Scene Details", &m_scene_inspector.window.settings);
		ImGui::InputText("Name", &settings.name);
		ImGui::InputText("Config", &settings.configPath);
		ImGui::InputText("Script", &settings.scriptPath);

		this->onInspectSettings(selected);

		ImGui::SeparatorText("world");

		EditorInspectSceneEntities(world, m_scene_inspector.selected);

		ImGui::End();
	}
	void iAppScene::inspectEntity(Entity& selected)
	{
		if (!selected)
			return;

		if (!m_scene_inspector.window.entity)
			return;

		ImGui::Begin("Entity Details", &m_scene_inspector.window.entity);

		selected.inspect();

		this->onInspectEntity(selected);

		ImGui::End();
	}
	void iAppScene::inspectViewport(Entity& selected)
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

		this->onInspectViewportTop(selected);

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

		this->onInspectViewportBottom(&viewportPos.x, &viewportSize.x, selected);

		ImGui::End();
		ImGui::PopStyleVar();
	}
}