#pragma once
#include "resource_types.hpp"
#include "types/texture_type.hpp"
#include "types/font_type.hpp"

#include <memory>
#include <unordered_map>
#include <typeindex>


namespace lib
{
	class ResourceRegistry;

	template<typename T>
	class ResourceHandle
	{
		UUID handle;
		ResourceRegistry* owner;
	public:
		ResourceHandle() : handle(0), owner(nullptr) {}
		ResourceHandle(UUID p_handle, ResourceRegistry* p_owner) : handle(p_handle), owner(p_owner) {}

		T* Get() const {
			if (owner == nullptr)
				return nullptr;
			return owner->GetResource<T>(handle);
		}

		bool IsValid() const {
			return owner && owner->HasResource<T>(handle);
		}

		void Release() {
			if (IsValid())
			{
				owner->Release<T>(handle);
			}
			owner = nullptr;
		}

		const UUID& GetUUID() const {
			return handle;
		}

		T* operator->() const {
			return Get();
		}
		T& operator*() const {
			return *Get();
		}

		operator bool() const {
			return IsValid();
		}

	};

	class ResourceRegistry 
	{
		struct ResourceData {
			std::shared_ptr<SceneResource> resource;
			int refCount;
		};

		std::unordered_map<std::type_index,
			std::unordered_map<UUID, std::shared_ptr<SceneResource>>> resources;

		std::unordered_map<std::type_index,
			std::unordered_map<UUID, ResourceData>> refCounts;

		std::unordered_map<std::type_index,
			std::unordered_map<std::string, UUID>> resourcePathLookup;

		friend struct ResourceRegistryBrowser;
		friend class ResourceManager;
	public:
		template<typename T>
		ResourceHandle<T> Load(const std::string& resourcePath, UUID id)
		{
			static_assert(std::is_base_of<SceneResource, T>::value, "T must derive from Resource");
			auto type_hash = std::type_index(typeid(T));

			auto& typeResources = resources[type_hash];
			auto& typePaths     = resourcePathLookup[type_hash];

			auto resource_it = typeResources.find(id);
			auto path_it = typePaths.find(resourcePath);

			//if id is zero, we assume here that the resource has never been seen before
			if (id == 0)
			{
				//search for an id
				if (path_it != typePaths.end())
				{
					id = path_it->second;
					resource_it = typeResources.find(id);
				}
				else
				{
					id = UUID();
				}
			}

			if (resource_it != typeResources.end())
			{
				refCounts[type_hash][id].refCount++;
				return ResourceHandle<T>(id, this);
			}

			//create a resource and attempt to load it

			auto resource = std::make_shared<T>(resourcePath, id);
			resource->load();
			if (!resource->isValid())
				return ResourceHandle<T>();

			typeResources[id] = resource;
			typePaths[resourcePath] = id;
			refCounts[type_hash][id].refCount = 1;

			return ResourceHandle<T>(id, this);
		}

		template<typename T>
		T* GetResource(UUID id)
		{
			auto& typeResources = resources[std::type_index(typeid(T))];
			auto it = typeResources.find(id);
			if (it != typeResources.end())
				return static_cast<T*>(it->second.get());

			return nullptr;
		}

		template<typename T>
		ResourceHandle<T> GetResourceHandle(UUID id, bool addRef = true)
		{
			auto& typeResources = resources[std::type_index(typeid(T))];
			auto it = typeResources.find(id);
			if (it != typeResources.end()) {
				if(addRef)
					refCounts[std::type_index(typeid(T))][id].refCount++;

				return ResourceHandle<T>(id, this);
			}

			return ResourceHandle<T>();
		}


		template<typename T>
		bool HasResource(UUID id)
		{
			// Efficient existence check without resource access overhead
			auto resourceIt = resources.find(std::type_index(typeid(T)));
			return resourceIt != resources.end();
		}

		template<typename T>
		void Release(UUID id)
		{
			// Locate reference count entry for this resource
			auto it = refCounts.find(id);
			if (it != refCounts.end()) {
				it->second--;

				// Check if resource has no remaining references
				if (it->second <= 0) {
					// Step 5a: Locate and unload the unreferenced resource across all type containers
					for (auto& [type, typeResources] : resources) {
						auto resourceIt = typeResources.find(id);
						if (resourceIt != typeResources.end()) {
							resourcePathLookup[type].erase(resourceIt->second->path); //remove path from cache
							resourceIt->second->free();      // Allow resource to clean up its data
							typeResources.erase(resourceIt);   // Remove from cache
							break;
						}
					}

					// Step 5b: Clean up reference counting entry
					refCounts.erase(it);
				}
			}
		}

		void UnloadAll() {
			// Emergency cleanup method for system shutdown or major state changes
			for (auto& [type, typeResources] : resources) {
				for (auto& [id, resource] : typeResources) {
					resource->free();     // Ensure all resources clean up properly
				}
				typeResources.clear();      // Clear type-specific containers
			}
			refCounts.clear();              // Reset all reference counts
			resourcePathLookup.clear();		// Reset all path caches
		}

		size_t size() const;
		SceneResource* getResourceAtIndex(int i);
		SceneResource* GetResourcePtr(UUID id);
	};
}