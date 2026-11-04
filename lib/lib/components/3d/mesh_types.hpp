#pragma once
#include "lib/core/uuid.hpp"
#include "transform3d.hpp"
#include "lib/resources/resource_manager.hpp"

namespace lib
{
    struct StaticMesh
    {
        bool visible = true;
        ModelHandle handle;
        Color tint = WHITE;

        StaticMesh();
        StaticMesh(const StaticMesh& o) = default;

        void render(const Transform3D& transform) const;
        static void Render(const Transform3D& transform, const StaticMesh& component);


        void write(YAML::Emitter& out);
        void read(const YAML::Node& node);
        void inspect();
        static void Bind(sol::state& lua);
        static void Inspect(Entity e, StaticMesh& component);
    };
}