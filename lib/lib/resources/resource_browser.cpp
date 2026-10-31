#include "resource_browser.hpp"
#include <tinyfiledialogs.h>
#include "resource_manager.hpp"

namespace lib
{
   
    static bool DoImportFont()
    {
        bool added = false;
        const char* aTitle = "Import Font File";
        const char* aDefaultPathAndFile = GetWorkingDirectory();
        int const aNumOfFilterPatterns = 2;
        const char* aFilterPatterns[] = { "*.ttf","*.otf" };
        const char* aSingleFilterDescription = "Font files";
        int const aAllowMultipleSelects = 0;

        char const* file = tinyfd_openFileDialog(
            aTitle,
            aDefaultPathAndFile,
            aNumOfFilterPatterns,
            aFilterPatterns,
            aSingleFilterDescription,
            aAllowMultipleSelects);

        if (file)
        {
            ResourceManager::GetSingleton().AddFont(file);
            added = true;
        }
        return added;
    }

    void ResourceBrowser::HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    void ResourceBrowser::DrawMenu(ResourceRegistry* registry, bool* p_open)
    {
        ImGui::BeginMenuBar();

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Import Texture"))
                ImportTexture(registry);

            if (ImGui::MenuItem("Import Sprite Animation", nullptr, nullptr, false))
                ImportSpriteAnimation(registry);

            if (ImGui::MenuItem("Import Font"))
                ImportFont(registry);

            if (ImGui::MenuItem("Import Model", nullptr, nullptr, false))
                ImportModel(registry);

            if (ImGui::MenuItem("Import Model Animation", nullptr, nullptr, false))
                ImportModelAnimation(registry);

            if (ImGui::MenuItem("Import Material", nullptr, nullptr, false))
                ImportMaterial(registry);

            if (ImGui::MenuItem("Import Shader", nullptr, nullptr, false))
                ImportShader(registry);

            ImGui::Separator();
            if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                *p_open = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Delete", "Del", false, false))
                RequestDelete = true;

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

            ImGui::SeparatorText("Contents");
            ImGui::Checkbox("Show Type Overlay", &ShowTypeOverlay);
            ImGui::Checkbox("Allow Sorting", &AllowSorting);

            ImGui::SeparatorText("Selection Behavior");
            ImGui::Checkbox("Allow box-selection", &AllowBoxSelect);
            if (ImGui::Checkbox("Allow box-selection from selected items", &AllowBoxSelectInsideSelection) && AllowBoxSelectInsideSelection)
                AllowDragUnselected = false;
            if (ImGui::Checkbox("Allow dragging unselected item", &AllowDragUnselected) && AllowDragUnselected)
                AllowBoxSelectInsideSelection = false;

            ImGui::SeparatorText("Layout");
            ImGui::SliderFloat("Icon Size", &IconSize, 16.0f, 128.0f, "%.0f");
            ImGui::SameLine(); HelpMarker("Use Ctrl+Wheel to zoom");
            ImGui::SliderInt("Icon Spacing", &IconSpacing, 0, 32);
            ImGui::SliderInt("Icon Hit Spacing", &IconHitSpacing, 0, 32);
            ImGui::Checkbox("Stretch Spacing", &StretchSpacing);
            ImGui::Checkbox("Use ScrollX", &UseScrollX);
            ImGui::PopItemWidth();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    void ResourceBrowser::Draw(ResourceRegistry* registry, bool* p_open)
    {
        DrawMenu(registry, p_open);
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowContentSize(
            ImVec2(0.0f, LayoutOuterPadding + LayoutLineCount * (LayoutItemSize.y + LayoutItemSpacing)));

        if (ImGui::BeginChild("Assets", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            const float avail_width = ImGui::GetContentRegionAvail().x;
            UpdateLayoutSizes(registry, avail_width);

            // Calculate and store start position.
            ImVec2 start_pos = ImGui::GetCursorScreenPos();
            start_pos = ImVec2(start_pos.x + LayoutOuterPadding, start_pos.y + LayoutOuterPadding);
            ImGui::SetCursorScreenPos(start_pos);

            ImGuiMultiSelectFlags ms_flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_ClearOnClickVoid;
            ms_flags |= ImGuiMultiSelectFlags_NavWrapX;

            ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(ms_flags, 0, registry->size());

            const int item_curr_idx_to_focus = -1;


            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(LayoutSelectableSpacing, LayoutSelectableSpacing));

            // Rendering parameters
            const ImU32 icon_type_overlay_colors[3] = { 0, IM_COL32(200, 70, 70, 255), IM_COL32(70, 170, 70, 255) };
            const ImU32 icon_bg_color = ImGui::GetColorU32(IM_COL32(35, 35, 35, 220));
            const ImVec2 icon_type_overlay_size = ImVec2(4.0f, 4.0f);
            const bool display_label = (LayoutItemSize.x >= ImGui::CalcTextSize("999").x);

            const int column_count = LayoutColumnCount;
            ImGuiListClipper clipper;
            clipper.Begin(LayoutLineCount, LayoutItemStep.y);
            if (item_curr_idx_to_focus != -1)
                clipper.IncludeItemByIndex(item_curr_idx_to_focus / column_count); // Ensure focused item line is not clipped.
            if (ms_io->RangeSrcItem != -1)
                clipper.IncludeItemByIndex((int)ms_io->RangeSrcItem / column_count); // Ensure RangeSrc item line is not clipped.


            while (clipper.Step())
            {
                for (int line_idx = clipper.DisplayStart; line_idx < clipper.DisplayEnd; line_idx++)
                {
                    const int item_min_idx_for_current_line = line_idx * column_count;
                    const int item_max_idx_for_current_line = std::min((line_idx + 1) * column_count, (int)registry->size());
                    for (int item_idx = item_min_idx_for_current_line; item_idx < item_max_idx_for_current_line; ++item_idx)
                    {
                        auto* item_data = registry->getResourceAtIndex(item_idx);
                        ImGui::PushID((int)item_data->id);

                        // Position item
                        ImVec2 pos = ImVec2(start_pos.x + (item_idx % column_count) * LayoutItemStep.x, start_pos.y + line_idx * LayoutItemStep.y);
                        ImGui::SetCursorScreenPos(pos);

                        ImGui::SetNextItemSelectionUserData(item_idx);
                        bool item_is_selected = open_resources.contains(item_data->id);
                        bool item_is_visible = ImGui::IsRectVisible(LayoutItemSize);
                        if (ImGui::Selectable("", item_is_selected, ImGuiSelectableFlags_None, LayoutItemSize))
                        {
                            if (item_is_selected)
                                open_resources.erase(item_data->id);
                            else
                                open_resources.insert(item_data->id);
                        }

                        // Update our selection state immediately (without waiting for EndMultiSelect() requests)
                        // because we use this to alter the color of our text/icon.
                        if (ImGui::IsItemToggledSelection())
                            item_is_selected = !item_is_selected;

                        // Focus (for after deletion)
                        if (item_curr_idx_to_focus == item_idx)
                            ImGui::SetKeyboardFocusHere(-1);

                        // Drag and drop
                        //resource_t::DragSource(item_data);

                        // Render icon (a real app would likely display an image/thumbnail here)
                        // Because we use ImGuiMultiSelectFlags_BoxSelect2d, clipping vertical may occasionally be larger, so we coarse-clip our rendering as well.
                        if (item_is_visible)
                        {
                            ImVec2 box_min(pos.x - 1, pos.y - 1);
                            ImVec2 box_max(box_min.x + LayoutItemSize.x + 2, box_min.y + LayoutItemSize.y + 2); // Dubious
                            draw_list->AddRectFilled(box_min, box_max, icon_bg_color); // Background color
                            static ImVec2 uv_min = ImVec2(0, -1);
                            static ImVec2 uv_max = ImVec2(1, 0);

                            if (item_data->type == SceneResourceType_Texture)
                            {
                                Texture2D preview_texture = ((TextureResource*)item_data)->data;
                                ImTextureID texture_id = (ImTextureID)(intptr_t)preview_texture.id;
                                draw_list->AddImage(texture_id, box_min, box_max, uv_min, uv_max, IM_COL32_WHITE);
                            }
                            else if (ShowTypeOverlay && item_data->type != SceneResourceType_Invalid)
                            {
                                ImU32 type_col = icon_type_overlay_colors[item_data->type % IM_COUNTOF(icon_type_overlay_colors)];
                                draw_list->AddRectFilled(ImVec2(box_max.x - 2 - icon_type_overlay_size.x, box_min.y + 2), ImVec2(box_max.x - 2, box_min.y + 2 + icon_type_overlay_size.y), type_col);
                            }
                            if (display_label)
                            {
                                ImU32 label_col = ImGui::GetColorU32(item_is_selected ? ImGuiCol_Text : ImGuiCol_TextDisabled);
                                char label[32];
                                sprintf(label, "%s", item_data->name.c_str());
                                draw_list->AddText(ImVec2(box_min.x, box_max.y - ImGui::GetFontSize()), label_col, label);
                            }
                        }

                        ImGui::PopID();
                    }
                }
            }
            clipper.End();

            if (registry->size() == 0)
                ImGui::Dummy(ImVec2(0, 0));
            ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing

            ms_io = ImGui::EndMultiSelect();

            HandleIconResizing(registry, start_pos, avail_width, io);
        }
        ImGui::EndChild();
    }
    void ResourceBrowser::UpdateLayoutSizes(ResourceRegistry* registry, float avail_width)
    {
        // Layout: when not stretching: allow extending into right-most spacing.
        LayoutItemSpacing = (float)IconSpacing;
        if (StretchSpacing == false)
            avail_width += floorf(LayoutItemSpacing * 0.5f);

        // Layout: calculate number of icon per line and number of lines
        LayoutItemSize = ImVec2(floorf(IconSize), floorf(IconSize));
        LayoutColumnCount = std::max((int)(avail_width / (LayoutItemSize.x + LayoutItemSpacing)), 1);

        // Layout: when stretching: allocate remaining space to more spacing. Round before division, so item_spacing may be non-integer.
        if (StretchSpacing && LayoutColumnCount > 1)
            LayoutItemSpacing = floorf(avail_width - LayoutItemSize.x * LayoutColumnCount) / LayoutColumnCount;

        if (UseScrollX)
            LayoutColumnCount *= 2;
        LayoutLineCount = (registry->size() + LayoutColumnCount - 1) / LayoutColumnCount;

        LayoutItemStep = ImVec2(LayoutItemSize.x + LayoutItemSpacing, LayoutItemSize.y + LayoutItemSpacing);
        LayoutSelectableSpacing = std::max(floorf(LayoutItemSpacing) - IconHitSpacing, 0.0f);
        LayoutOuterPadding = floorf(LayoutItemSpacing * 0.5f);
    }
    void ResourceBrowser::HandleIconResizing(ResourceRegistry* registry, ImVec2 start_pos, float avail_width, ImGuiIO& io)
    {
        // Zooming with Ctrl+Wheel
        if (ImGui::IsWindowAppearing())
            ZoomWheelAccum = 0.0f;
        if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f && ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsAnyItemActive() == false)
        {
            ZoomWheelAccum += io.MouseWheel;
            if (fabsf(ZoomWheelAccum) >= 1.0f)
            {
                // Calculate hovered item index from mouse location
                // FIXME: Locking aiming on 'hovered_item_idx' (with a cool-down timer) would ensure zoom keeps on it.
                const float hovered_item_nx = (io.MousePos.x - start_pos.x + LayoutItemSpacing * 0.5f) / LayoutItemStep.x;
                const float hovered_item_ny = (io.MousePos.y - start_pos.y + LayoutItemSpacing * 0.5f) / LayoutItemStep.y;
                const int hovered_item_idx = ((int)hovered_item_ny * LayoutColumnCount) + (int)hovered_item_nx;
                //ImGui::SetTooltip("%f,%f -> item %d", hovered_item_nx, hovered_item_ny, hovered_item_idx); // Move those 4 lines in block above for easy debugging

                // Zoom
                IconSize *= powf(1.1f, (float)(int)ZoomWheelAccum);
                IconSize = std::clamp(IconSize, 16.0f, 128.0f);
                ZoomWheelAccum -= (int)ZoomWheelAccum;
                UpdateLayoutSizes(registry, avail_width);

                // Manipulate scroll to that we will land at the same Y location of currently hovered item.
                // - Calculate next frame position of item under mouse
                // - Set new scroll position to be used in next ImGui::BeginChild() call.
                float hovered_item_rel_pos_y = ((float)(hovered_item_idx / LayoutColumnCount) + fmodf(hovered_item_ny, 1.0f)) * LayoutItemStep.y;
                hovered_item_rel_pos_y += ImGui::GetStyle().WindowPadding.y;
                float mouse_local_y = io.MousePos.y - ImGui::GetWindowPos().y;
                ImGui::SetScrollY(hovered_item_rel_pos_y - mouse_local_y);
            }
        }
    }
    void ResourceBrowser::ImportTexture(ResourceRegistry* registry)
    {
        const char* aTitle = "Import Image File";
        const char* aDefaultPathAndFile = GetWorkingDirectory();
        int const aNumOfFilterPatterns = 2;
        const char* aFilterPatterns[] = { "*.jpg","*.png" };
        const char* aSingleFilterDescription = "image files";
        int const aAllowMultipleSelects = 0;

        char const* file = tinyfd_openFileDialog(
            aTitle,
            aDefaultPathAndFile,
            aNumOfFilterPatterns,
            aFilterPatterns,
            aSingleFilterDescription,
            aAllowMultipleSelects);

        if (file)
        {
            registry->Load<TextureResource>(file, 0);
        }
    }
    void ResourceBrowser::ImportSpriteAnimation(ResourceRegistry* registry)
    {
    }
    void ResourceBrowser::ImportFont(ResourceRegistry* registry)
    {
        const char* aTitle = "Import Font File";
        const char* aDefaultPathAndFile = GetWorkingDirectory();
        int const aNumOfFilterPatterns = 2;
        const char* aFilterPatterns[] = { "*.ttf","*.otf" };
        const char* aSingleFilterDescription = "Font files";
        int const aAllowMultipleSelects = 0;

        char const* file = tinyfd_openFileDialog(
            aTitle,
            aDefaultPathAndFile,
            aNumOfFilterPatterns,
            aFilterPatterns,
            aSingleFilterDescription,
            aAllowMultipleSelects);

        if (file)
        {
            registry->Load<FontResource>(file, 0);
        }
    }
    void ResourceBrowser::ImportModel(ResourceRegistry* registry)
    {
    }
    void ResourceBrowser::ImportModelAnimation(ResourceRegistry* registry)
    {
    }
    void ResourceBrowser::ImportMaterial(ResourceRegistry* registry)
    {
    }
    void ResourceBrowser::ImportShader(ResourceRegistry* registry)
    {
    }
}