#pragma once
#include <lib/core/scene/iAppScene.hpp>
#include "lib/utils/yaml_common.hpp"

namespace lib
{
	typedef iScene* (*SceneGenerationFunction)(SceneSettings p_settings);
	typedef iAppScene* (*AppSceneGenerationFunction)(const SceneSettings& p_settings);

	enum SceneType
	{
		SceneType_2D,
		SceneType_3D,
		SceneType_Custom,
	};
	struct SceneEntry
	{
		int type = SceneType_2D;
		std::string name, path;

		inline SceneSettings toSceneSettings() const {
			return SceneSettings{
				.name = name,
				.configPath = path,
			};
		}
	};
	struct AppState
	{
		struct WindowSettings
		{
			int width = 1280;
			int height = 720;
			int flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
			int monitor = 0;
			int fps = 120;
		};
		struct SceneEntryList
		{
			int current = 0;
			std::vector<SceneEntry> entries;
			SceneEntry GetCurrentEntry() const;
		};
		
		std::string name = "Application";
		WindowSettings window;
		SceneEntryList scenes;
		std::vector<SceneGenerationFunction> generators;

		bool isRunning = true;
		bool isPaused = false;
		bool canRestartScene = false;
		bool canChangeScene  = false;

		void load(const std::string& filepath);
		void save(const std::string& filepath);

		void initApp();
		void shutdownApp();
		
		void LoadNewScene(iScene** scene, const SceneEntry& entry);
		//   returns true if the application should close
		bool PollForStateChanges(iScene** scene);
		void renderImGuiMainMenu(iScene* activeScene);
	};
}