#include "texture_type.hpp"
#include <imgui.h>

#ifndef IM_MIN
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

// Enforce cdecl calling convention for functions called by the standard library,
// in case compilation settings changed the default to e.g. __vectorcall
#ifndef IMGUI_CDECL
#ifdef _MSC_VER
#define IMGUI_CDECL __cdecl
#else
#define IMGUI_CDECL
#endif
#endif
#endif // !IM_MIN


namespace lib
{
    static bool IsImageType(const char* filepath)
    {
        bool result = false;
        const char* fileType = GetFileExtension(filepath);
        if ((false)
            || (strcmp(fileType, ".png") == 0) || (strcmp(fileType, ".PNG") == 0)
            || (strcmp(fileType, ".bmp") == 0) || (strcmp(fileType, ".BMP") == 0)
            || (strcmp(fileType, ".tga") == 0) || (strcmp(fileType, ".TGA") == 0)
            || (strcmp(fileType, ".jpg") == 0) || (strcmp(fileType, ".JPG") == 0)
            || (strcmp(fileType, ".jpeg") == 0)|| (strcmp(fileType, ".JPEG") == 0)
            || (strcmp(fileType, ".gif") == 0) || (strcmp(fileType, ".GIF") == 0)
            || (strcmp(fileType, ".pic") == 0) || (strcmp(fileType, ".PIC") == 0)
            || (strcmp(fileType, ".ppm") == 0) || (strcmp(fileType, ".pgm") == 0)
            || (strcmp(fileType, ".PPM") == 0) || (strcmp(fileType, ".PGM") == 0)
            || (strcmp(fileType, ".psd") == 0) || (strcmp(fileType, ".PSD") == 0)
            )
        {
            result = true;
        }

        return result;
    }

    struct ImageViewerData
    {
        ImU32   ImageBgColor = IM_COL32(100, 100, 100, 255);
        ImU32   GridColor = IM_COL32(255, 255, 255, 100);
        bool    GridEnabled = true;
        bool    ViewReset = true;
        ImVec2  ViewOffset; // in image space
        float   Zoom = 1.0f;
        float   ZoomMin = 0.5f;
        float   ZoomMax = 10000.0f;
    };

    static void ImageViewer_DrawOptions(ImageViewerData* data)
    {
        ImGui::SetNextItemShortcut(ImGuiKey_G, ImGuiInputFlags_Tooltip); // | ImGuiInputFlags_RouteGlobal
        ImGui::Checkbox("Grid", &data->GridEnabled);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 10.0f);
        float zoom_100 = data->Zoom * 100.0f;
        if (ImGui::DragFloat("##Zoom", &zoom_100, 5.0f, data->ZoomMin * 100.0f, data->ZoomMax * 100.0f, "%.0f%%", ImGuiSliderFlags_AlwaysClamp))
            data->Zoom = zoom_100 / 100.0f;
    }

    static void ImageViewer_DrawCanvas(ImageViewerData* data, ImVec2 canvas_size, ImTextureRef image_tex_ref, int image_w, int image_h)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        IM_ASSERT(canvas_size.x >= 0.0f && canvas_size.y >= 0.0f);

        // Layout canvas
        ImGui::InvisibleButton("##Canvas", canvas_size);
        ImVec2 canvas_min = ImGui::GetItemRectMin();
        ImVec2 canvas_max = ImGui::GetItemRectMax();

        if (data->ViewReset)
            data->ViewOffset = ImVec2((canvas_size.x * 0.5f / data->Zoom) - 0.5f, (canvas_size.y * 0.5f / data->Zoom) - 0.5f); // Add half a pixel padding
        data->ViewReset = false;

        // Handle inputs
        if (ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY))
            if (io.MouseWheel != 0.0f)
                data->Zoom = IM_CLAMP(data->Zoom * (1.0f + io.MouseWheel * 0.10f), data->ZoomMin, data->ZoomMax);
        float zoom = data->Zoom; // (float)(int)ViewZoom;
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
        {
            data->ViewOffset.x -= io.MouseDelta.x / zoom;
            data->ViewOffset.y -= io.MouseDelta.y / zoom;
        }

        // Display image
        ImVec2 image_min, image_max;
        image_min.x = (float)(int)((canvas_min.x - (data->ViewOffset.x * zoom)) + (canvas_size.x * 0.5f));
        image_min.y = (float)(int)((canvas_min.y - (data->ViewOffset.y * zoom)) + (canvas_size.y * 0.5f));
        image_max.x = (float)(int)(image_min.x + image_w * zoom);
        image_max.y = (float)(int)(image_min.y + image_h * zoom);
        draw_list->AddRect(ImVec2(canvas_min.x - 1.0f, canvas_min.y - 1.0f), ImVec2(canvas_max.x + 1.0f, canvas_max.y + 1.0f), IM_COL32(255, 255, 255, 255));
        draw_list->PushClipRect(canvas_min, canvas_max, true);
        draw_list->AddRectFilled(image_min, image_max, data->ImageBgColor);
        if (platform_io.DrawCallback_SetSamplerNearest != NULL)
            draw_list->AddCallback(platform_io.DrawCallback_SetSamplerNearest);
        draw_list->AddImage(image_tex_ref, image_min, image_max);
        if (platform_io.DrawCallback_SetSamplerLinear != NULL)
            draw_list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear);

        // Display grid lines for visible pixels
        if (data->GridEnabled && zoom > 6.0f)
        {
            const float step = (float)zoom;
            for (int px = (int)((canvas_min.x - image_min.x) / step); px <= (int)((canvas_max.x - image_min.x) / step); px++)
                draw_list->AddLineV(image_min.x + px * step, canvas_min.y, canvas_max.y, data->GridColor, 1.0f);
            for (int py = (int)((canvas_min.y - image_min.y) / step); py <= (int)((canvas_max.y - image_min.y) / step); py++)
                draw_list->AddLineH(canvas_min.x, canvas_max.x, image_min.y + py * step, data->GridColor, 1.0f);
        }
        draw_list->PopClipRect();
    }

    void TextureResource::load()
    {
        if (!path.empty())
        {
            data = ::LoadTexture(path.c_str());
            if (IsImageType(path.c_str()))
            {
                TraceLog(LOG_WARNING, "Set Texture resource to load image data or resource.file data");
            }
            else //assume yaml/engine type
            {
                TraceLog(LOG_ERROR, "Set Texture resource to load image data or resource.file data");
            }
        }

        if (isValid())
        {
            SetTextureFilter(data, filter);
            SetTextureWrap(data, wrap);
        }
    }

    void TextureResource::free()
    {
        if (isValid())
            UnloadTexture(data);

        data = {0};
    }

    bool TextureResource::isValid() const
    {
        return IsTextureValid(data);
    }

    void TextureResource::setFilter(int p_filter)
    {
        filter = p_filter;
        SetTextureFilter(data, filter);
    }

    void TextureResource::setWrap(int p_wrap)
    {
        wrap = p_wrap;
        SetTextureWrap(data, wrap);
    }

    void TextureResource::onRead(const YAML::Node& node)
    {
        readYamlValue(node["filter"], &filter);
        readYamlValue(node["wrap"], &wrap);
    }

    void TextureResource::onWrite(YAML::Emitter& out) const
    {
        out << YAML::Key << "filter" << YAML::Value << filter;
        out << YAML::Key << "wrap" << YAML::Value << wrap;
    }

    void TextureResource::onInspectWindow()
    {
        if (ImGui::RadioButton("Point", filter == TEXTURE_FILTER_POINT)) { setFilter(TEXTURE_FILTER_POINT); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Bilinear", filter == TEXTURE_FILTER_BILINEAR)) { setFilter(TEXTURE_FILTER_BILINEAR); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Trilinear", filter == TEXTURE_FILTER_TRILINEAR)) { setFilter(TEXTURE_FILTER_TRILINEAR); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Anisotropic 4x", filter == TEXTURE_FILTER_ANISOTROPIC_4X)) { setFilter(TEXTURE_FILTER_ANISOTROPIC_4X); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Anisotropic 8x", filter == TEXTURE_FILTER_ANISOTROPIC_8X)) { setFilter(TEXTURE_FILTER_ANISOTROPIC_8X); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Anisotropic 16x", filter == TEXTURE_FILTER_ANISOTROPIC_16X)) { setFilter(TEXTURE_FILTER_ANISOTROPIC_16X); }

        if (ImGui::RadioButton("Repeat", wrap == TEXTURE_WRAP_REPEAT)) setWrap(TEXTURE_WRAP_REPEAT);
        ImGui::SameLine();
        if (ImGui::RadioButton("Clamp", wrap == TEXTURE_WRAP_CLAMP)) setWrap(TEXTURE_WRAP_CLAMP);
        ImGui::SameLine();
        if (ImGui::RadioButton("Mirror Repeat", wrap == TEXTURE_WRAP_MIRROR_REPEAT)) setWrap(TEXTURE_WRAP_MIRROR_REPEAT);
        ImGui::SameLine();
        if (ImGui::RadioButton("Mirror Clamp", wrap == TEXTURE_WRAP_MIRROR_CLAMP)) setWrap(TEXTURE_WRAP_MIRROR_CLAMP);


        static ImageViewerData image_viewer;

        ImVec2 canvas_size(ImGui::GetContentRegionAvail().x, data.width * 2.0f);
        ImageViewer_DrawOptions(&image_viewer);
        ImageViewer_DrawCanvas(&image_viewer, canvas_size, data.id, (int)data.width, (int)data.height);

        //todo: draw image @ 100% resolution by default
    }
        
}