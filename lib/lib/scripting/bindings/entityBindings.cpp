#include "lib/scripting/luaScriptBindings.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"

namespace lib::scripting
{
    void BindEntity(sol::state& lua)
    {
        lua.new_usertype<Entity>("Entity"
            , "type_id", &entt::type_hash<Entity>::value
            , sol::call_constructor, sol::factories([]() { return Entity(); })
            , "destroy", &Entity::destroy
            , "getName", &Entity::getName
            , "getUUID", &Entity::getUUID
            , "isEnabled", &Entity::isEnabled
            , "isVisible", &Entity::isVisible
            , "isSerializable", &Entity::isSerializable
            , "enable", &Entity::enable
            , "disable", &Entity::disable
            , "show", &Entity::show
            , "hide", &Entity::hide
            , "disableSerialization", &Entity::disableSerialization
            , "enableSerialization", &Entity::enableSerialization
            , "createChild", &Entity::createChild
            , "addParent", &Entity::addParent
            , "removeParent", &Entity::removeParent
            , "addChild", &Entity::addChild
            , "removeChild", &Entity::removeChild
            , "removeAllChildren", &Entity::removeAllChildren
            , "getFirstChild", &Entity::getFirstChild
            , "getNextSibling", &Entity::getNextSibling
            , "getPrevSibling", &Entity::getPrevSibling
            , "getParent", &Entity::getParent
            , "getChildNamed", &Entity::getChildNamed
            , "hasChild", &Entity::isChild
            , "isDecendantOf", &Entity::isDecendantOf
            , "hasParent", &Entity::hasParent
            , "isValid", &Entity::isValid
            , "getHandle", &Entity::getHandle
            , "getHandleInt32", &Entity::getHandleInt32
            //, "setPosition", &Entity::setPosition
            , "add", bind::entity_bind_emplace
            , "get", bind::entity_bind_get
            , "has", bind::entity_bind_has
            , "remove", bind::entity_bind_remove
        );

    }
}