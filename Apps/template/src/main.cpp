#include <lib/lib.hpp>
#include <raylib.h>

#include "lib/components/transform3d.hpp"
#include "lib/components/sceneCamera3d.hpp"
#include "lib/core/uuid.hpp"

#include "lib/utils/yaml_common.hpp"
#include "lib/utils/editor_utils.hpp"

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

	Model model = LoadModelFromMesh(GenMeshCube(1, 1, 1));
	lib::SceneCamera3D camera;

	lib::Transform3D transform;
	transform.position.y = 0.5f;
	bool show_imgui_demo = true;

	lib::EditorBegin();
	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();

		transform.RotateAroundAxis({ 0, 1, 0 }, 45 * dt);

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
			UpdateCamera(&camera.camera, CAMERA_THIRD_PERSON);


		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);

		model.transform = transform.toMatrix();
		DrawModel(model, { 0 }, 1.0f, WHITE);
		DrawGrid(10, 1.0f);
		EndMode3D();

		DrawText("Template", 10, 10, 20, RAYWHITE);
		lib::EditorBeginDraw();
		if (show_imgui_demo)
			ImGui::ShowDemoWindow(&show_imgui_demo);

		lib::EditorEndDraw();
		EndDrawing();
	}
	UnloadModel(model);
	config.save("app.config.yaml");
	lib::EditorEnd();
	CloseWindow();
	return 0;
}