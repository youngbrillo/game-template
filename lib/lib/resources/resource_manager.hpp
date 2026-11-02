#pragma once
#include "resource_registry.hpp"
#include "resource_browser.hpp"

namespace lib
{
	using TextureHandle = ResourceHandle<TextureResource>;
	using FontHandle    = ResourceHandle<FontResource>;

	class ResourceManager
	{
		ResourceRegistry registry;
	public:
		ResourceManager();
		~ResourceManager();
		static ResourceManager& GetSingleton();

		TextureHandle AddTexture(std::string filepath);
		TextureHandle GetTexture(UUID id);
		FontHandle AddFont(std::string filepath);
		FontHandle GetFont(UUID id);

		void FreeAll();
		inline ResourceRegistry* GetRegistry() { return &registry; }
	public:
		void RenderImGuiWindow(bool* isOpen, const char* label = "Resource Browser");

		bool DrawTextureComboBox(const char* label, UUID& id, Texture2D* data);
		bool DrawFontComboBox(const char* label, UUID& id, Font* data);


		void Read(const YAML::Node& root);
		void Write(YAML::Emitter& out);
		std::vector<SceneResource*>* resource_list = nullptr;
	};
}