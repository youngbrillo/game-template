#pragma once
#include "lib/core/entity.hpp"
#include <raylib.h>

namespace lib
{
	struct SceneSettings
	{
		std::string name;
		std::string configPath;
		std::string scriptPath;
	};

	class iScene
	{
	public:
		SceneSettings settings;
		entt::registry world;
	public: 
		iScene(SceneSettings p_settings)
			: settings(p_settings){}

		virtual ~iScene() {}

		virtual void init() = 0;
		virtual void free() = 0;
		virtual void update(float dt) = 0;
		virtual void fixedUpdate(float timestep) = 0;
		virtual void render() = 0;

		void SaveToFile(std::string filepath);
		void SaveToOutput(YAML::Emitter& out);

		void LoadFromFile(std::string filepath);
		void LoadFromOutput(const YAML::Node& root);
	protected:
		virtual void onDataSave(YAML::Emitter& root) {}
		virtual void onDataLoad(const YAML::Node& root) {}
	};
}