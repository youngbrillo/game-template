#include <lib/lib.hpp>
#include <raylib.h>

#include "lib/components/3d/transform3d.hpp"
#include "lib/components/3d/sceneCamera3d.hpp"

#include "lib/utils/yaml_common.hpp"
#include "lib/utils/editor_utils.hpp"
#include "scenes_registration.h"
#include <vector>
#include "lib/core/input_manager.hpp"

#include "tinyfiledialogs.h"

namespace lib
{
	static iScene* CreateTemplateSceneWrapper(SceneSettings settings)
	{
		return CreateTemplateScene(settings);
	}
}

int main(int argc, char** argv)
{
	using namespace lib;
	AppState state;
	state.load("app.config.yaml");
	state.initApp();

	lib::SceneSettings settings = state
		.scenes
		.GetCurrentEntry()
		.toSceneSettings();

	state.generators[SceneType_2D] = CreateTemplateSceneWrapper;
	state.generators[SceneType_3D] = CreateTemplateSceneWrapper;
	state.generators[SceneType_Custom] = CreateTemplateSceneWrapper;

	iScene* scene = nullptr;
	state.canChangeScene = true;

	float pTimer = 0.0f, pTimeLimit = 1.0f / 60.0f;
	while (state.isRunning)
	{
		if (state.PollForStateChanges(&scene))
			break;

		float dt = GetFrameTime();
		if (state.isPaused)
			dt = 0.0f;

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
			state.renderImGuiMainMenu(scene);
			scene->inspect();
		lib::EditorEndDraw();
		EndDrawing();
	}
	scene->free();
	delete scene;
	scene = nullptr;

	state.save("app.config.yaml");
	state.shutdownApp();
	return 0;
}