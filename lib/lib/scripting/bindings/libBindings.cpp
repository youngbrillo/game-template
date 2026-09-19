#include "lib/scripting/luaScriptBindings.hpp"
#include <raylib.h>

#include "lib/components/transform3d.hpp"

namespace lib::scripting
{
    void BindLib(sol::state& lua)
    {
        Transform3D::ScriptBind(lua);

    }
}