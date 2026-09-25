#include "lib/scripting/luaScriptBindings.hpp"
#include "lib/components/components3d.hpp"

namespace lib::scripting
{
    void BindLib(sol::state& lua)
    {
        Transform3D::ScriptBind(lua);
        StaticMesh::Bind(lua);
        Rigidbody3D::Bind(lua);
        iCollider3D::Bind(lua);

        SceneCamera3D::Bind(lua);
    }
}