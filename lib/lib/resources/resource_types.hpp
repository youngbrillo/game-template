#pragma once
#include <lib/core/uuid.hpp>
#include <lib/utils/yaml_common.hpp>

namespace lib
{
	enum SceneResourceType{
		SceneResourceType_Invalid,
		SceneResourceType_Texture,
		SceneResourceType_SpriteAnimation,
		SceneResourceType_Font,
		SceneResourceType_Model,
		SceneResourceType_ModelAnimation,
		SceneResourceType_Mesh,
		SceneResourceType_Material,
		SceneResourceType_Shader,
		SceneResourceType_Count
	};

	class SceneResource
	{
	public:
		int type = SceneResourceType_Invalid;
		UUID id = 0;
		std::string name, path;

		void read(const YAML::Node& node);
		void write(YAML::Emitter& out, bool flow = true) const;

		void saveToFile(const std::string& filepath) const;
		void loadFromFile(const std::string& filepath);
		void inspectWindow(bool* isOpen);

		virtual void load() = 0;
		virtual void free() = 0;
		virtual bool isValid() const = 0;
	protected:
		virtual void onRead(const YAML::Node& node) {}
		virtual void onWrite(YAML::Emitter& out) const {}
		virtual void onInspectWindow() {}
	};
}