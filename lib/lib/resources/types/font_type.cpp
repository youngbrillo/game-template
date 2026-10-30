#include "font_type.hpp"
#include <imgui.h>
namespace lib
{
    void FontResource::load()
    {
        if (!path.empty())
        {
            data = ::LoadFontEx(path.c_str(), size, nullptr, 0);
        }

        if (isValid())
        {
            SetTextureFilter(data.texture, filter);
        }
    }

    void FontResource::free()
    {
        if (isValid())
        {
            UnloadFont(data);
            data = { 0 };
        }
    }

    bool FontResource::isValid() const
    {
        return IsFontValid(data);
    }

    void FontResource::setFilter(int p_filter)
    {
        filter = p_filter;
        SetTextureFilter(data.texture, filter);
    }

    void FontResource::setSize(int p_size)
    {
        size = p_size;
        if (isValid())
        {
            this->free();
            this->load();
        }
    }

    void FontResource::onRead(const YAML::Node& node)
    {
        readYamlValue(node["size"], &size);
        readYamlValue(node["filter"], &filter);
    }

    void FontResource::onWrite(YAML::Emitter& out) const
    {
        out << YAML::Key << "size" << YAML::Value << size;
        out << YAML::Key << "filter" << YAML::Value << filter;
    }

    void FontResource::onInspectWindow()
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

        if (ImGui::InputFloat("size", &size))
        {
            this->setSize(size);
        }

    }
}