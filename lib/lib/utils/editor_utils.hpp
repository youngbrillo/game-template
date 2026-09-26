#pragma once
#include <raylib.h>
#include <imgui.h>
#include "lib/core/entity.hpp"

namespace lib
{
	void EditorBegin();

	void EditorEnd();

	void EditorBeginDraw();

	void EditorEndDraw();

	void EditorInspectSceneEntities(entt::registry& world, Entity& selected_entity);
	
	void EditorInspectEntityInHierarchy(Entity e, Entity& selected_entity);

	bool EditorDrawTransformWidget(ImVec2 viewportPos, ImVec2 viewportSize, const Camera& camera, Matrix& matrix);
}