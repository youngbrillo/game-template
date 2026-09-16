#pragma once
#include <raylib.h>
#include <imgui.h>

namespace lib
{
	void EditorBegin();

	void EditorEnd();

	void EditorBeginDraw();

	void EditorEndDraw();

	bool EditorDrawTransformWidget(ImVec2 viewportPos, ImVec2 viewportSize, const Camera& camera, Matrix& matrix);
}