#include "resource_registry.hpp"
#include <imgui.h>

namespace lib
{
	size_t ResourceRegistry::size() const
    {
        size_t count = 0;
        for (auto& [type, typeResources] : resources) {
            for (auto& [id, resource] : typeResources) {
                count++;
            }
        }

        return count;
    }
    SceneResource* ResourceRegistry::getResourceAtIndex(int i)
    {
        int count = 0;
        for (auto& [type, typeResources] : resources) {
            for (auto& [id, resource] : typeResources) {
                if (i == count)
                {
                    return resource.get();
                }
                count++;
            }
        }
        return nullptr;
    }
    SceneResource* ResourceRegistry::GetResourcePtr(UUID uuid)
    {
        for (auto& [type, typeResources] : resources) {
            for (auto& [id, resource] : typeResources) {
                if (id == uuid)
                {
                    return resource.get();
                }
            }
        }
        return nullptr;
    }
}