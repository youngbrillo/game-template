#pragma once
#include "lib/core/uuid.hpp"
#include "raylib.h"
#include "transform3d.hpp"

namespace lib
{
    struct StaticMesh
    {
        UUID id = 0;
        Color tint = WHITE;

        void write(YAML::Emitter& out);
        void read(const YAML::Node& node);
        void inspect();
        static void Bind(sol::state& lua);
        static void Inspect(Entity e, StaticMesh& component);
    };
}