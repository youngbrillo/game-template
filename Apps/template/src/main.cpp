#include <lib/lib.hpp>
#include <raylib.h>

#include "lib/components/transform3d.hpp"
#include "lib/components/sceneCamera3d.hpp"

#include "lib/utils/yaml_common.hpp"
#include "lib/utils/editor_utils.hpp"
#include "scenes_registration.h"

struct AppConfig
{
	std::string name = "Template";
	lib::UUID id;

	struct 
	{
		int width = 1280;
		int height = 720;
		int flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
		int monitor = 0;
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
					//<< YAML::Key << "lockFPS" << YAML::Value << window.lockFPS
					//<< YAML::Key << "FPS" << YAML::Value << window.FPS
					<< YAML::Key << "monitor" << YAML::Value << window.monitor
				<< YAML::EndMap
				//<< YAML::Key << "memory" << YAML::Value
				//<< YAML::BeginMap
				//	<< YAML::Key << "rememberLastMonitor" << YAML::Value << memory.lastMonitor
				//	<< YAML::Key << "rememberScreenSize" << YAML::Value << memory.screenSize
				//<< YAML::EndMap
				//<< YAML::Key << "config" << YAML::Value
				//<< YAML::BeginMap
				//	<< YAML::Key << "logLevel" << YAML::Value << config.logLevel
				//	<< YAML::Key << "escapeKey" << YAML::Value << config.escapeKey
				//	<< YAML::Key << "clearColor" << YAML::Value << TextFormat("0x%08" PRIx32, config.clearColor)
				//<< YAML::EndMap
			<< YAML::EndMap
			;

		lib::SaveYamlFile(filepath, out);
	}
};



int main(int argc, char** argv)
{
	AppConfig config = AppConfig::LoadFromFile("app.config.yaml");
	SetConfigFlags(config.window.flags);
	InitWindow(config.window.width, config.window.height, config.name.c_str());
	if (config.window.monitor < GetMonitorCount())
	{
		SetWindowMonitor(config.window.monitor);
		SetWindowSize(config.window.width, config.window.height);
	}
	SetTargetFPS(60);

	bool show_imgui_demo = true;
	lib::EditorBegin();

	lib::SceneSettings settings = {
		.name = "Template",
		.configPath = "resources/scene1.yaml"
	};


	lib::iScene* scene = CreateTemplateScene(settings);

	scene->init();
	float pTimer = 0.0f, pTimeLimit = 1.0f / 60.0f;
	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();
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
		if (show_imgui_demo)
			ImGui::ShowDemoWindow(&show_imgui_demo);

		lib::EditorEndDraw();
		EndDrawing();
	}
	scene->free();
	delete scene;
	scene = nullptr;

	config.save("app.config.yaml");
	lib::EditorEnd();
	CloseWindow();
	return 0;
}