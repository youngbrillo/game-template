#include <lib/lib.hpp>
#include <raylib.h>

#include "lib/components/transform3d.hpp"
#include "lib/components/sceneCamera3d.hpp"

#include "lib/utils/yaml_common.hpp"
#include "lib/utils/editor_utils.hpp"
#include "scenes_registration.h"
#include <vector>

struct AppConfig
{
	struct SceneEntry
	{
		std::string name, path;
	};

	std::string name = "Template";
	lib::UUID id;

	struct {
		int current = 0;
		std::vector<SceneEntry> entries;
	} scenes;

	struct 
	{
		int width = 1280;
		int height = 720;
		int flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
		int monitor = 0;
		int fps = 120;
	} window;

	static AppConfig LoadFromFile(const char* filepath)
	{
		AppConfig a;
		YAML::Node root = lib::LoadYamlFile(filepath);

		if (root.IsMap())
		{
			if (auto node = root["app"])
			{
				lib::readYamlValue(node["name"], &a.name);
				lib::readYamlValue(node["id"], &a.id);
				if (auto win_node = node["window"])
				{
					lib::readYamlValue(win_node["width"], &a.window.width);
					lib::readYamlValue(win_node["height"], &a.window.height);
					lib::readYamlValue(win_node["flags"], &a.window.flags);
					lib::readYamlValue(win_node["monitor"], &a.window.monitor);
					lib::readYamlValue(win_node["fps"], &a.window.fps);
				}

				if (auto scenes_node = node["scenes"])
				{
					lib::readYamlValue(scenes_node["current"], &a.scenes.current);

					for (auto n : scenes_node["entries"])
					{
						if (auto s = n["scene"])
						{
							auto& entry = a.scenes.entries.emplace_back();
							lib::readYamlValue(s["name"], &entry.name);
							lib::readYamlValue(s["path"], &entry.path);
						}
					}
				}
			}
		}
		return a;
	}
	void save(const char* filepath)
	{
		window.width = GetScreenWidth();
		window.height = GetScreenHeight();
		window.monitor = GetCurrentMonitor();

		YAML::Emitter out;

		out << YAML::BeginMap
			<< YAML::Key << "app" << YAML::Value
			<< YAML::BeginMap
				<< YAML::Key << "name" << YAML::Value << name
				<< YAML::Key << "id" << YAML::Value << id
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
								<< YAML::Key << "name" << YAML::Value << i.name
								<< YAML::Key << "path" << YAML::Value << i.path
							<< YAML::EndMap
						<< YAML::EndMap
						;
					}

		out			<< YAML::EndSeq
			<< YAML::EndMap

			<< YAML::EndMap
			;

		lib::SaveYamlFile(filepath, out);
	}
	
	void init()
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

	}

	void shutdown()
	{
		lib::EditorEnd();
		CloseWindow();
	}

	const SceneEntry& GetCurrentEntry() const {
		return scenes.entries[scenes.current];
	}

	void renderMainMenu(lib::Scene3D* scene)
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New Scene", nullptr, nullptr, false);
			ImGui::MenuItem("Open Scene", nullptr, nullptr, false);

			if (ImGui::MenuItem("Save Scene", "CTRL+S", nullptr, scene != nullptr))
			{
				scene->SaveToFile(scene->settings.configPath);
			}
			if (ImGui::MenuItem("Save Scene As...", "CTRL+SHIFT+S", nullptr, scene != nullptr))
			{
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Quit", "ALT+F4"))
			{

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Restart Scene", nullptr, nullptr, scene != nullptr))
			{
				scene->free();
				scene->init();
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
					scene->free();
					scene->settings.name = this->GetCurrentEntry().name;
					scene->settings.configPath = this->GetCurrentEntry().path;

					scene->init();
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
};

int main(int argc, char** argv)
{
	AppConfig config = AppConfig::LoadFromFile("app.config.yaml");
	config.init();

	lib::SceneSettings settings = {
		.name = config.GetCurrentEntry().name,
		.configPath = config.GetCurrentEntry().path,
	};


	auto* scene = CreateTemplateScene(settings);

	scene->init();
	float pTimer = 0.0f, pTimeLimit = 1.0f / 60.0f;
	bool paused = false;
	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();
		if (paused)
			dt = 0.0f;

		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_R))
		{
			scene->free();
			scene->init();
		}

		if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_P))
		{
			paused = !paused;
		}

		scene->update(dt);

		pTimer += dt;
		if (pTimer >= pTimeLimit)
		{
			pTimer = 0.0f;
			scene->fixedUpdate(pTimeLimit);
		}

		BeginDrawing();
		ClearBackground(BLACK);
		scene->render();

		lib::EditorBeginDraw();
			config.renderMainMenu(scene);
			scene->inspect();
		lib::EditorEndDraw();
		EndDrawing();
	}
	scene->free();
	delete scene;
	scene = nullptr;

	config.save("app.config.yaml");
	config.shutdown();
	return 0;
}