#include "appState.hpp"
#include <lib/utils/yaml_common.hpp>
#include <lib/utils/editor_utils.hpp>
#include "tinyfiledialogs.h"
#include "lib/core/input_manager.hpp"

namespace lib
{
	void AppState::load(const std::string& filepath)
	{
		YAML::Node root = lib::LoadYamlFile(filepath);

		if (root.IsMap())
		{
			if (auto node = root["app"])
			{
				readYamlValue(node["name"], &name);
				if (auto win_node = node["window"])
				{
					lib::readYamlValue(win_node["width"], &window.width);
					lib::readYamlValue(win_node["height"], &window.height);
					lib::readYamlValue(win_node["flags"], &window.flags);
					lib::readYamlValue(win_node["monitor"], &window.monitor);
					lib::readYamlValue(win_node["fps"], &window.fps);
				}

				if (auto scenes_node = node["scenes"])
				{
					lib::readYamlValue(scenes_node["current"], &scenes.current);

					for (auto n : scenes_node["entries"])
					{
						if (auto s = n["scene"])
						{
							auto& entry = scenes.entries.emplace_back();
							lib::readYamlValue(s["type"], &entry.type);
							lib::readYamlValue(s["name"], &entry.name);
							lib::readYamlValue(s["path"], &entry.path);
						}
					}
				}
			}
		}
	}

	void AppState::save(const std::string& filepath)
	{
		window.width = GetScreenWidth();
		window.height = GetScreenHeight();
		window.monitor = GetCurrentMonitor();

		YAML::Emitter out;

		out << YAML::BeginMap
			<< YAML::Key << "app" << YAML::Value
			<< YAML::BeginMap
			<< YAML::Key << "name" << YAML::Value << name
			<< YAML::Key << "window" << YAML::Value
			<< YAML::BeginMap
			<< YAML::Key << "width" << YAML::Value << window.width
			<< YAML::Key << "height" << YAML::Value << window.height
			<< YAML::Key << "flags" << YAML::Value << window.flags
			<< YAML::Key << "fps" << YAML::Value << window.fps
			<< YAML::Key << "monitor" << YAML::Value << window.monitor
			<< YAML::EndMap
			<< YAML::Key << "scenes" << YAML::Value
			<< YAML::BeginMap
			<< YAML::Key << "current" << YAML::Value << scenes.current
			<< YAML::Key << "entries" << YAML::Value
			<< YAML::BeginSeq;
		for (auto& i : scenes.entries)
		{
			out << YAML::BeginMap
				<< YAML::Key << "scene" << YAML::Value
				<< YAML::Flow << YAML::BeginMap
				<< YAML::Key << "type" << YAML::Value << i.type
				<< YAML::Key << "name" << YAML::Value << i.name
				<< YAML::Key << "path" << YAML::Value << i.path
				<< YAML::EndMap
				<< YAML::EndMap
				;
		}

		out << YAML::EndSeq
			<< YAML::EndMap

			<< YAML::EndMap
			;

		lib::SaveYamlFile(filepath, out);
	}

	void AppState::initApp()
	{
		SetConfigFlags(window.flags);
		InitWindow(window.width, window.height, name.c_str());
		if (window.monitor < GetMonitorCount())
		{
			SetWindowMonitor(window.monitor);
			SetWindowSize(window.width, window.height);
		}
		SetTargetFPS(window.fps);
		lib::EditorBegin();


		generators = { nullptr, nullptr, nullptr };
	}

	void AppState::shutdownApp()
	{
		lib::EditorEnd();
		CloseWindow();
	}

	void AppState::LoadNewScene(iScene** scene, const SceneEntry& entry)
	{
		if (scene)
		{
			if (*scene) {
				(*scene)->free();
				delete* scene;
				*scene = nullptr;
			}

			*scene = generators[entry.type](entry.toSceneSettings());
			(*scene)->init();
		}
		canRestartScene = false;
		canChangeScene = false;
	}

	bool AppState::PollForStateChanges(iScene** scene)
	{
		if (WindowShouldClose())
			isRunning = false;

		if (*scene && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_R))
		{
			canRestartScene = true;
		}

		if ((IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_P)) || InputManager::isActionReleased(ActionPause))
		{
			isPaused = !isPaused;
		}

		if (canRestartScene && scene)
		{
			(*scene)->free();
			(*scene)->init();
			canRestartScene = false;
		}

		if (canChangeScene)
		{
			LoadNewScene(scene, scenes.GetCurrentEntry());
		}

		return !isRunning;
	}

	void AppState::renderImGuiMainMenu(iScene* scene)
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", nullptr, nullptr))
			{
				scenes.entries.push_back(SceneEntry{
					.name = "New Scene", .path = "NewScene.yml" });

				scenes.current = scenes.entries.size() - 1;
				canChangeScene = true;
			}

			if (ImGui::MenuItem("Open Scene", nullptr, nullptr))
			{
				char* nextFile;
				char const* filterPatterns[2] = { "*.yaml", "*.yml" };
				nextFile = tinyfd_openFileDialog(
					"Load a scene file",
					GetWorkingDirectory(), 
					2, 
					filterPatterns, 
					"scene files", 
					0
				);
				if (nextFile)
				{
					auto& new_entry = this->scenes.entries.emplace_back();
					new_entry.name = GetFileNameWithoutExt(nextFile);
					new_entry.path = nextFile;

					int i = this->scenes.entries.size() - 1;

					scenes.current = i;
					canChangeScene = true;

				}
			}

			if (ImGui::MenuItem("Save Scene", "CTRL+S", nullptr, scene != nullptr))
			{
				scene->SaveToFile(scene->settings.configPath);
				scenes.entries[scenes.current].name = scene->settings.name;
				scenes.entries[scenes.current].path = scene->settings.configPath;
			}
			if (ImGui::MenuItem("Save Scene As...", "CTRL+SHIFT+S", nullptr, scene != nullptr))
			{
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Quit", "ALT+F4"))
			{
				isRunning = false;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Restart Scene", nullptr, nullptr, scene != nullptr))
			{
				canRestartScene = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scenes"))
		{
			for (int i = 0; i < scenes.entries.size(); i++)
			{
				if (ImGui::MenuItem(TextFormat("%s", scenes.entries[i].name.c_str())
					, nullptr, i == scenes.current, scene != nullptr))
				{
					scenes.current = i;
					canChangeScene = true;
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}



	SceneEntry AppState::SceneEntryList::GetCurrentEntry() const
	{
		if (entries.size() == 0)
			return SceneEntry{
			.type = SceneType_2D, 
			.name = "brand new scene", 
			.path = "brand_new_scene.yaml"
		};

		return entries[current];
	}
}