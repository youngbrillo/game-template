#pragma once
#include <entt/entt.hpp>
#include "uuid.hpp"
#include <string>

namespace YAML
{
    class Emitter;
    class Node;
}
namespace lib
{
    class Entity;

    typedef void(*SerializeEntityCallback)(YAML::Emitter& out, Entity& e);
    typedef void(*DeserializeEntityCallback)(const YAML::Node& root, Entity& e);
    typedef void(*EntityComponentPopupCallback)(Entity& e);
    typedef void(*EntityComponentWidgetCallback)(Entity& e);
    typedef void(*EntityActionPopupCallback)(entt::registry& world, Entity& selected_entity);

    void SetSerializeEntityCallback(SerializeEntityCallback callback);
    void SetDeserializeEntityCallback(DeserializeEntityCallback callback);
    void SetEntityComponentPopupCallback(EntityComponentPopupCallback callback);
    void SetEntityComponentWidgetCallback(EntityComponentWidgetCallback callback);
    void SetEntityActionPopupCallback(EntityActionPopupCallback callback);

    namespace components
    {
        struct NameTag { std::string name = "entity"; };

        struct IDTag { UUID tag = 0; };

        struct HiddenTag { char x; };

        struct DisabledTag { char x; };

        struct DeleteTag { char x; };

        struct DoNotSerializeTag { char x; };

        struct Relationship
        {
            entt::entity parent{ entt::null };
            entt::entity firstChild{ entt::null };
            entt::entity prevSib{ entt::null };
            entt::entity nextSib{ entt::null };
        };

    }
    class Entity
    {
        entt::entity _handle = entt::null;
        entt::registry* _world = nullptr;
    public:
        Entity() = default;

        Entity(entt::entity handle, entt::registry& regsitry) : _handle(handle), _world(&regsitry) {}

        Entity(entt::entity handle, entt::registry* regsitry) : _handle(handle), _world(regsitry) {}

        Entity(const Entity&) = default;

        /// @brief Destroy an entity and all it's children (recursive). Unparents entity from parent.
        void destroy();

        std::string getName();

        UUID getUUID();

        bool isEnabled();

        void enable();

        void disable();

        bool isVisible();

        void hide();

        void show();

        bool isSerializable();

        void disableSerialization();

        void enableSerialization();

        Entity duplicate();

        void Serialize(YAML::Emitter& out);

        void Deserialize(const YAML::Node& node);

        void inspect();

        Entity createChild(std::string name);

        void addParent(Entity parent);

        void removeParent();

        void addChild(Entity child);

        void removeChild(Entity child);

        void removeAllChildren();

        Entity getFirstChild();

        Entity getNextSibling();

        Entity getPrevSibling();

        Entity getParent();

        Entity getChildNamed(std::string name);

        bool isChild(Entity child);

        bool hasParent();

        bool isDecendantOf(Entity ancestor);

        uint32_t getNumberOfChildren();



        /// @brief check if entity has component 'T'
        /// @tparam T : component
        /// @return : returns true if entity contains T
        template <typename T>
        inline bool has();

        /// @brief Add Component 'T' to entity. Will throw if entity already possesses the component
        /// @tparam T 
        /// @tparam ...Args constructor arguments
        /// @param ...args 
        /// @return 
        template <typename T, typename... Args>
        inline T& add(Args&&... args);

        /// @brief Add Component 'T' to enitty. Will not throw if entity already possesses the component
        /// @tparam T 
        /// @tparam ...Args 
        /// @param ...args 
        /// @return 
        template <typename T, typename... Args>
        inline T& addOrReplace(Args&&... args);

        /// @brief Get Component 'T', Will throw if enitty does not possess the component.
        /// @tparam T 
        /// @return 
        template <typename T>
        inline T& get();

        /// @brief Try to Get Component 'T'. 
        /// @tparam T 
        /// @return retuns a pointer to the component if found, else returns a nullptr
        template <typename T>
        inline T* tryGet();

        /// @brief remove a compnent of type 'T'. Will not throw if if component is not found
        /// @tparam T 
        template <typename T>
        inline void remove();


        bool operator==(const Entity& other) const
        {
            return _handle == other._handle && _world == other._world;
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

        bool isValid() const { return _handle != entt::null && _world != nullptr; }

        uint32_t        getHandleInt32() const { return (uint32_t)_handle; }

        entt::entity    getHandle() const { return _handle; }

        operator bool() const { return _handle != entt::null && _world != nullptr; }

        operator entt::entity() const { return _handle; }

        operator uint32_t() const { return (uint32_t)_handle; }

        entt::registry* getWorld() const { return _world; }


        static Entity Create(entt::registry& world, std::string name);
        static void DragSource(const char* label, Entity e);
        static bool DropTarget(Entity& e, entt::registry& world);
        static void HierarchyPopupActions(entt::registry& world, Entity& selected_entity);
    };


    template<typename T>
    inline bool Entity::has()
    {
        return _world->try_get<T>(_handle) != nullptr;
    }
    template<typename T, typename ...Args>
    inline T& Entity::add(Args && ...args)
    {
        return _world->emplace<T>(_handle, std::forward<Args>(args)...);
    }
    template<typename T, typename ...Args>
    inline T& Entity::addOrReplace(Args && ...args)
    {
        return _world->emplace_or_replace<T>(_handle, std::forward<Args>(args)...);
    }
    template<typename T>
    inline T& Entity::get()
    {
        return _world->get<T>(_handle);
    }
    template<typename T>
    inline T* Entity::tryGet()
    {
        return _world->try_get<T>(_handle);
    }
    template<typename T>
    inline void Entity::remove()
    {
        if (has<T>())
        {
            _world->remove<T>(_handle);
        }
    }

}

