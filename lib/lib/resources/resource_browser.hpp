#pragma once
#include "resource_manager.hpp"
#include <imgui.h>

namespace lib
{
    using resource_set= std::set<UUID>;

    struct ResourceBrowser
    {
        // Options
        bool            ShowTypeOverlay = true;
        bool            AllowSorting = false;
        bool            AllowBoxSelect = true;                  // Will set ImGuiMultiSelectFlags_BoxSelect2d
        bool            AllowBoxSelectInsideSelection = false;  // Will set ImGuiMultiSelectFlags_SelectOnClickAlways
        bool            AllowDragUnselected = true;            // Will set ImGuiMultiSelectFlags_SelectOnClickRelease
        float           IconSize = 64;
        int             IconSpacing = 18;
        int             IconHitSpacing = 4;                     // Increase hit-spacing if you want to make it possible to clear or box-select from gaps. Some spacing is required to able to amend with Shift+box-select. Value is small in Explorer.
        bool            StretchSpacing = true;
        bool            UseScrollX = false;                     // Debug: submit twice the number of items per line (overflow horizontally to exercise ScrollX + box-select)

        // State
        ImGuiID         NextItemId = 0;             // Unique identifier when creating new items
        bool            RequestDelete = false;      // Deferred deletion request
        bool            RequestSort = false;        // Deferred sort request
        float           ZoomWheelAccum = 0.0f;      // Mouse wheel accumulator to handle smooth wheels better

        // Calculated sizes for layout, output of UpdateLayoutSizes(). Could be locals but our code is simpler this way.
        ImVec2          LayoutItemSize;
        ImVec2          LayoutItemStep;             // == LayoutItemSize + LayoutItemSpacing
        float           LayoutItemSpacing = 0.0f;
        float           LayoutSelectableSpacing = 0.0f;
        float           LayoutOuterPadding = 0.0f;
        int             LayoutColumnCount = 0;
        int             LayoutLineCount = 0;

        resource_set open_resources;

        void HelpMarker(const char* desc);
        void DrawMenu(ResourceRegistry* registry, bool* p_open);
        void Draw(ResourceRegistry* registry, bool* p_open);


        //helpers
        void UpdateLayoutSizes(ResourceRegistry* registry, float avail_width);
        void HandleIconResizing(ResourceRegistry* registry, ImVec2 start_pos, float avail_width, ImGuiIO& io);

        //importers
        void ImportTexture(ResourceRegistry* registry);
        void ImportSpriteAnimation(ResourceRegistry* registry);
        void ImportFont(ResourceRegistry* registry);
        void ImportModel(ResourceRegistry* registry);
        void ImportModelAnimation(ResourceRegistry* registry);
        void ImportMaterial(ResourceRegistry* registry);
        void ImportShader(ResourceRegistry* registry);

    };
}