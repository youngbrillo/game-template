#include "texture_type.hpp"
#include <imgui.h>

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
            || (strcmp(fileType, ".jpg") == 0) || (strcmp(fileType, ".jpeg") == 0)
            || (strcmp(fileType, ".JPG") == 0) || (strcmp(fileType, ".JPEG") == 0)
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
        if (ImGui::RadioButton("Trilinear", filter == TEXTURE_FILTER_TRILINEAR)) { setFilter(TEXTURE_FILTER_TRILINEAR); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Anisotropic 4x", filter == TEXTURE_FILTER_ANISOTROPIC_4X)) { setFilter(TEXTURE_FILTER_ANISOTROPIC_4X); }
        if (ImGui::RadioButton("Anisotropic 8x", filter == TEXTURE_FILTER_ANISOTROPIC_8X)) { setFilter(TEXTURE_FILTER_ANISOTROPIC_8X); }
        ImGui::SameLine();
        if (ImGui::RadioButton("Anisotropic 16x", filter == TEXTURE_FILTER_ANISOTROPIC_16X)) { setFilter(TEXTURE_FILTER_ANISOTROPIC_16X); }

        if (ImGui::RadioButton("Repeat", wrap == TEXTURE_WRAP_REPEAT)) setWrap(TEXTURE_WRAP_REPEAT);
        ImGui::SameLine();
        if (ImGui::RadioButton("Clamp", wrap == TEXTURE_WRAP_CLAMP)) setWrap(TEXTURE_WRAP_CLAMP);
        if (ImGui::RadioButton("Mirror Repeat", wrap == TEXTURE_WRAP_MIRROR_REPEAT)) setWrap(TEXTURE_WRAP_MIRROR_REPEAT);
        ImGui::SameLine();
        if (ImGui::RadioButton("Mirror Clamp", wrap == TEXTURE_WRAP_MIRROR_CLAMP)) setWrap(TEXTURE_WRAP_MIRROR_CLAMP);


        //todo: draw image @ 100% resolution by default
    }
        
}