#include "resource_manager.hpp"
#include <imgui.h>
#include "resource_browser.hpp"

namespace lib
{
	ResourceManager::ResourceManager()
	{

	}
	ResourceManager::~ResourceManager()
	{
	}
	ResourceManager& ResourceManager::GetSingleton()
	{
		static ResourceManager instance;
		return instance;
	}
	TextureHandle ResourceManager::AddTexture(std::string filepath)
	{
		TextureHandle res = registry.Load<TextureResource>(filepath, 0);
		return res;
	}
	TextureHandle ResourceManager::GetTexture(UUID id)
	{
		TextureHandle res = registry.GetResourceHandle<TextureResource>(id);
		return res;
	}
	FontHandle ResourceManager::AddFont(std::string filepath)
	{
		FontHandle res = registry.Load<FontResource>(filepath, 0);
		return res;
	}

	FontHandle ResourceManager::GetFont(UUID id)
	{
		FontHandle res = registry.GetResourceHandle<FontResource>(id);
		return res;
	}

	void ResourceManager::FreeAll()
	{
		registry.UnloadAll();
	}

	void ResourceManager::RenderImGuiWindow(bool* isOpen, const char* label)
	{
		static ResourceBrowser browser;

		ImGuiWindowFlags_ flags = ImGuiWindowFlags_MenuBar;
		if (!ImGui::Begin(label, isOpen, flags))
		{
			ImGui::End();
		}
		else
		{
			browser.Draw(&registry, isOpen);
			ImGui::End();
		}


		for (auto i : browser.open_resources)
		{
			auto* r = registry.GetResourcePtr(i);
			bool open = true;
			r->inspectWindow(&open);
			if (!open)
			{
				browser.open_resources.erase(i);
				break;
			}
		}
	}

	using resource_map = std::unordered_map<std::type_index, std::unordered_map<UUID, std::shared_ptr<SceneResource>>>;

	template <typename T>
	static bool DrawResourceComboBox(const char* label, UUID& id, resource_map& map, 
		bool is_default_available, bool* is_default_selected = nullptr, UUID default_uuid = 0)
	{
		bool updated = false;

		const char* preview_value = "<Default>";
		auto& typeResources = map[std::type_index(typeid(T))];

		if (!typeResources.empty() && id != 0)
		{
			preview_value = typeResources[id]->name.c_str();
		}

		if (ImGui::BeginCombo(label, preview_value))
		{
			if (is_default_available)
			{
				if (ImGui::Selectable("<Default>", id == default_uuid))
				{
					id = default_uuid;
					updated = true;
					if (is_default_selected)
					{
						*is_default_selected = true;
					}
				}
			}

			for (auto&& [i, resource] : typeResources)
			{
				bool is_selected = i == id;

				if (ImGui::Selectable(resource->name.c_str(), is_selected))
				{
					id = i;

					updated = true;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return updated;
	}


	bool ResourceManager::DrawTextureComboBox(const char* label, UUID& id, Texture2D* data)
	{
		bool chose_default = false;
		bool updated = DrawResourceComboBox<TextureResource>(label, id, registry.resources, true, &chose_default);

		if (updated && data)
		{
			if (chose_default)
			{
				*data = Texture2D{ 1,1,1,};
			}
			else
			{
				auto* r = registry.GetResource<TextureResource>(id);
				*data = r->data;
			}
		}

		return updated;
	}
	bool ResourceManager::DrawFontComboBox(const char* label, UUID& id, Font* data)
	{
		bool chose_default = false;
		bool updated = DrawResourceComboBox<FontResource>(label, id, registry.resources, true, &chose_default);

		if (updated && data)
		{
			if (chose_default)
			{
				*data = GetFontDefault();
			}
			else
			{
				auto* r = registry.GetResource<FontResource>(id);
				*data = r->data;
			}
		}

		return updated;
	}
}