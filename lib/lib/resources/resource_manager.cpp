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
	bool ResourceManager::DrawTextureComboBox(const char* label, UUID& id, Texture2D* data)
	{
		bool updated = false;

		const char* preview_value = "<Default>";
		auto& typeResources = registry.resources[std::type_index(typeid(TextureResource))];

		if (!typeResources.empty() && id != 0)
		{
			preview_value = typeResources[id]->name.c_str();
		}

		if (ImGui::BeginCombo(label, preview_value))
		{
			for (auto&& [i, resource] : typeResources)
			{
				bool is_selected = i == id;

				if (ImGui::Selectable(resource->name.c_str(), is_selected))
				{
					id = i;

					if (data) {
						TextureResource* r = (TextureResource*)(resource.get());
						*data = r->data;
					}

					updated = true;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return updated;
	}
	bool ResourceManager::DrawFontComboBox(const char* label, UUID& id, Font* data)
	{
		bool updated = false;

		const char* preview_value = "<Default>";
		auto& typeResources = registry.resources[std::type_index(typeid(TextureResource))];

		if (!typeResources.empty() && id != 0)
		{
			preview_value = typeResources[id]->name.c_str();
		}

		if (ImGui::BeginCombo(label, preview_value))
		{
			for (auto&& [i, resource] : typeResources)
			{
				if (resource->type != SceneResourceType_Font)
					continue;

				bool is_selected = i == id;

				if (ImGui::Selectable(resource->name.c_str(), is_selected))
				{
					id = i;

					if (data) {
						FontResource* r = (FontResource*)(resource.get());
						*data = r->data;
					}

					updated = true;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return updated;
	}
}