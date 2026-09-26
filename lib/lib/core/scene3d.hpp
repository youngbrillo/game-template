#pragma once 
#include "lib/core/iScene.hpp"
#include "lib/components/sceneCamera3d.hpp"
#include "lib/scripting/luaScript.hpp"
#include <box3d/box3d.h>

namespace lib
{
	struct SceneInspector
	{
		struct {
			bool settings = true;
			bool entity = true;
			bool viewport = true;
			bool demo = false;
		} window;
		Entity selected;
	};

	class Scene3D : public iScene
	{
	public:
		SceneCamera3D default_camera;
		LuaScript mainScript;
		b3WorldId worldId;
		SceneViewport viewport;
		SceneInspector m_scene_inspector;
	public:
		Scene3D(SceneSettings p_settings);
		virtual ~Scene3D();

		virtual void init();
		virtual void free();
		virtual void update(float dt);
		virtual void fixedUpdate(float timestep);
		virtual void render();
		virtual void renderScene();
		virtual void inspect();

		const SceneCamera3D& getSceneCamera() const;
	protected:
		virtual void onInit() {}
		virtual void onScriptInitalized() {}
		virtual void onFree() {}
		virtual void onUpdate(float dt) {}
		virtual void onFixedUpdate(float timestep) {}
		virtual void onRender3D(const SceneCamera3D& camera) {}
		virtual void onRenderUI() {}
		virtual void onInspectWindow() {}
		virtual void onInspectSettings() {}
	protected:
		void inspectSettings(Entity& selected);
		void inspectEntity(Entity& selected);
		void inspectViewport(Entity& selected);
	};
}