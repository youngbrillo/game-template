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


	//entity inspection utilities

	template <typename T>
	static void RenderComponentPopupMenuItem(const char* label, Entity e)
	{
		if (e.has<T>() == false)
		{
			if (ImGui::MenuItem(label))
			{
				e.add<T>();
			}
		}
	}
	template<typename T>
	void EntityQuickInspctor(Entity& e, T& component)
	{
		component.inspect();
	}

	template<typename T, typename UIFunction, typename ...Args>
	inline void RenderComponentEditWidget(const char* title, Entity& entity, UIFunction uifunction,
		Args && ...args)
	{
		if (!entity.has<T>())
			return;
		bool removeComponent = false;
		ImVec2 contentRegionAvaliable = ImGui::GetContentRegionAvail();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
		float lineheight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(),
			ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowOverlap
			, title);

		ImGui::SameLine(contentRegionAvaliable.x - lineheight * 0.5f);

		if (ImGui::Button("+", ImVec2(lineheight, lineheight)))
		{
			ImGui::OpenPopup("component Settings");
		}
		ImGui::PopStyleVar();

		if (ImGui::BeginPopup("component Settings"))
		{
			if (ImGui::MenuItem("Remove Component"))
			{
				removeComponent = true;;
			}

			ImGui::EndPopup();
		}

		if (open)
		{

			ImGui::AlignTextToFramePadding();
			ImGui::PushItemWidth(-150);
			uifunction(entity, entity.get<T>(), std::forward<Args>(args)...);

			ImGui::TreePop();
		}
		if (removeComponent)
		{
			entity.remove<T>();
		}
	}
}