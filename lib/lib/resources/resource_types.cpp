#include "resource_types.hpp"
#include <imgui.h>
#include <imgui_stdlib.h>

namespace lib
{
    SceneResource::SceneResource(std::string p_path, UUID p_id, SceneResourceType p_type)
        : id(p_id)
        , path(p_path)
        , type(p_type)
    {
        if (!path.empty())
        {
            name = GetFileNameWithoutExt(path.c_str());
        }
    }
    void SceneResource::read(const YAML::Node& node)
    {
        if (node)
        {
            readYamlValue(node["type"], &type);
            readYamlValue(node["id"], &id);
            readYamlValue(node["name"], &name);
            readYamlValue(node["path"], &path);

            this->onRead(node);
        }
    }

    void SceneResource::write(YAML::Emitter& out, bool flow) const
    {
        if(flow) out << YAML::Flow;
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << type;
        out << YAML::Key << "id" << YAML::Value << id;
        out << YAML::Key << "name" << YAML::Value << name;
        out << YAML::Key << "path" << YAML::Value << path;

        this->onWrite(out);

        out << YAML::EndMap;
    }

    void SceneResource::saveToFile(const std::string& filepath) const
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "resource";

        this->write(out, false);

        out << YAML::EndMap;

        SaveYamlFile(filepath, out);
        TraceLog(LOG_INFO, "Successfully Saved '%s' to '%s'", name.c_str(), filepath.c_str());
    }

    void SceneResource::loadFromFile(const std::string& filepath)
    {
        auto root = LoadYamlFile(filepath);
        this->read(root);
        TraceLog(LOG_INFO, "Successfully Loaded '%s' to '%s'", name.c_str(), filepath.c_str());
    }

    void SceneResource::inspectWindow(bool* isOpen)
    {
        std::string window_name = name;

        if (!ImGui::Begin(window_name.c_str(), isOpen))
        {
            ImGui::End();
            return;
        }

        ImGui::Text("type: %d | %s", type, id.to_str().c_str());
        ImGui::InputText("name", &name);
        ImGui::BeginDisabled();
            ImGui::InputText("path", &path);
        ImGui::EndDisabled();

        if (this->isValid())
        {
            if (ImGui::Button("reload"))
            {
                this->free();
                this->load();
            }
        }

        this->onInspectWindow();

        ImGui::End();
    }
    void parse_scene_parameters(const YAML::Node& node, int& type, std::string& path, std::string& name, UUID& id)
    {
        readYamlValue(node["type"], &type);
        readYamlValue(node["id"], &id);
        readYamlValue(node["name"], &name);
        readYamlValue(node["path"], &path);
    }
}