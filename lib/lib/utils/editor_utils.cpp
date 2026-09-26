#include "editor_utils.hpp"
#include "rlImgui/rlImGui.h"
#include "imgui_stdlib.h"
#include "ImGuizmo/ImGuizmo.h"
#include <raymath.h>
#include "lib/utils/yaml_common.hpp"

namespace lib
{

    // Gizmo state variables
    static ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
    static ImGuizmo::MODE currentGizmoMode = ImGuizmo::LOCAL;
    static Vector3 gizmo_snap = { 0.0f, 0.0f, 0.0f };

    struct float16_t {
        float data[16] = { 0 };

        float16_t() = default;
        float16_t(const float16_t& o) = default;
        //row-major return
        float16_t(const Matrix& m)
        {
            data[0] = m.m0; data[4] = m.m4; data[8] = m.m8; data[12] = m.m12;
            data[1] = m.m1; data[5] = m.m5; data[9] = m.m9; data[13] = m.m13;
            data[2] = m.m2; data[6] = m.m6; data[10] = m.m10; data[14] = m.m14;
            data[3] = m.m3; data[7] = m.m7; data[11] = m.m11; data[15] = m.m15;
        }

        Matrix toMatrix() const
        {
            return Matrix{
                .m0 = data[0], .m4 = data[4], .m8 = data[8], .m12 = data[12],
                .m1 = data[1], .m5 = data[5], .m9 = data[9], .m13 = data[13],
                .m2 = data[2], .m6 = data[6], .m10 = data[10], .m14 = data[14],
                .m3 = data[3], .m7 = data[7], .m11 = data[11], .m15 = data[15],
            };
        }
    };
	void EditorBegin()
	{
		rlImGuiSetup(true);
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
	}

	void EditorEnd()
	{
		rlImGuiShutdown();
	}

	void EditorBeginDraw()
	{
		rlImGuiBegin();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::BeginFrame();
		ImGuizmo::Enable(true); //ensures imguizmo can pick up mouse events

#ifdef IMGUI_HAS_DOCK
		ImGui::DockSpaceOverViewport(0, NULL, ImGuiDockNodeFlags_PassthruCentralNode);
#endif
	}

	void EditorEndDraw()
	{
		rlImGuiEnd();
	}

    void EditorInspectSceneEntities(entt::registry& world, Entity& selected_entity)
    {
        auto view = world.view<entt::entity>();

        for (auto it = view.rbegin(); it != view.rend(); ++it) {
            Entity e(*it, world);
            if (e.hasParent())
                continue;
            EditorInspectEntityInHierarchy(e, selected_entity);
        }


		//draw menu when not over entity
		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_::ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::BeginMenu("Actions"))
			{
				if (ImGui::BeginMenu("Create Entities"))
				{
					if (ImGui::MenuItem("Empty"))
						selected_entity = Entity::Create(world, "blank entity");

					Entity::HierarchyPopupActions(world, selected_entity);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
    }

    void EditorInspectEntityInHierarchy(Entity e, Entity& selected_entity)
    {
		Entity child = e.getFirstChild();
		auto& info = e.get<components::NameTag>();

		int flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_DrawLinesFull
			//| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| (selected_entity == e ? ImGuiTreeNodeFlags_Selected : 0);

		bool hasChildren = child;

		if (hasChildren == false)
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
		uint32_t id = e;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)id, flags, info.name.c_str());
		// a bunch of other stuff
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			ImGui::SetTooltip("id: %d", id);

		if (ImGui::IsItemClicked())
		{
			if (e == selected_entity) {
				selected_entity = Entity();
			}
			else {
				selected_entity = e;
			}
		}

		Entity::DragSource("Reparent:", e);
		Entity pChild;

		if (Entity::DropTarget(pChild, *e.getWorld()))
		{
			std::string pchild_name = pChild.getName();
			std::string e_name = e.getName();
			pChild.addParent(e);
		}
		//render popup menu -------------------------------------------------------
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Inspect Components"))
			{
				if (selected_entity == e) {
					selected_entity = Entity();
				}
				else {
					selected_entity = e;
				}
			}

			if (ImGui::MenuItem("Duplicate", 0, nullptr))
			{
				selected_entity = e.duplicate();
			}

			if (ImGui::MenuItem("Copy To Clipboard", 0, nullptr))
			{
				YAML::Emitter output;
				e.Serialize(output);
				std::string input = output.c_str();
				SetClipboardText(input.c_str());
				TraceLog(LOG_INFO, "Copied Contents of %s to clipboard.", e.getName().c_str());;
			}
			if (ImGui::MenuItem("Unparent", 0, nullptr))
			{
				e.removeParent();
			}

			if (ImGui::MenuItem("Delete Entity"))
			{
				e.destroy();

				if (selected_entity == e)
				{
					selected_entity = Entity();

				}
			}
			ImGui::EndPopup();
		}

		//render children -------------------------------------------------------
		if (opened)
		{
			while (child)
			{
				EditorInspectEntityInHierarchy(child, selected_entity);
				if (child) 
					child = child.getNextSibling();
			}
			ImGui::TreePop();
		}
    }

	bool EditorDrawTransformWidget(
        ImVec2 viewportPos, 
        ImVec2 viewportSize, 
        const Camera& camera, 
        Matrix& p_transform
    )
	{
        bool v = false;

        bool isWindowhovered = ImGui::IsWindowHovered();

        bool can_operate = isWindowhovered && IsKeyDown(KEY_LEFT_SHIFT);

        if (can_operate)
        {
            // Hotkey operation toggles
            if (IsKeyPressed(KEY_W)) currentGizmoOperation = ImGuizmo::TRANSLATE;
            if (IsKeyPressed(KEY_E)) currentGizmoOperation = ImGuizmo::ROTATE;
            if (IsKeyPressed(KEY_R)) currentGizmoOperation = ImGuizmo::SCALE;

        }

        gizmo_snap = can_operate ? Vector3{ 0.5f, 0.5f, 0.5f } : Vector3{ 0.0f, 0.0f, 0.0f };


        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
        //ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
        ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

        // View Matrix: Use Raylib's MatrixLookAt
        float16_t view = MatrixLookAt(camera.position, camera.target, camera.up);
        // Projection Matrix: Match the sub-viewport's aspect ratio
        float aspect = viewportSize.x / viewportSize.y;
        float16_t proj = MatrixPerspective(camera.fovy * DEG2RAD, aspect, 0.1f, 1000.0f);

        float16_t mod = p_transform;

        // Draw hardcoded 3D test cubes at origin using ImGuizmo's primitive renderer
        //ImGuizmo::DrawCubes(view.data, proj.data, mod.data, 1);
        // 
        // Draw & manipulate gizmo handles
        if (ImGuizmo::Manipulate(view.data, proj.data, currentGizmoOperation, currentGizmoMode, mod.data, nullptr, &gizmo_snap.x))
        {
            p_transform = mod.toMatrix();
            v = true;
        }

        // Copy modified transform back to our Raylib matrix if interacted
        if (ImGuizmo::IsUsing())
        {
            v = true;
        }

        return v;
	}

}
