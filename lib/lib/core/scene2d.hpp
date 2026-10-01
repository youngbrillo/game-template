#pragma once 
#include "lib/core/iScene.hpp"
#include "lib/components/sceneCamera2d.hpp"
#include "lib/scripting/luaScript.hpp"

namespace lib
{
	class Scene2D : public iScene
	{
	protected:
		struct SceneDebug
		{
			struct {
				bool settings = true;
				bool entity = true;
				bool viewport = true;
				bool demo = false;
			} window;
			Entity selected;
		};
	public:
		SceneCamera2D default_camera;
		LuaScript mainScript;
		SceneViewport viewport;
		SceneDebug m_scene_inspector;
	public:
		Scene2D(SceneSettings p_settings);
		virtual ~Scene2D();

		virtual void init();
		virtual void free();
		virtual void update(float dt);
		virtual void fixedUpdate(float timestep);
		virtual void render();
		virtual void renderScene();
		virtual void inspect();

		const SceneCamera2D& getSceneCamera() const;
	protected:
		virtual void onInit() {}
		virtual void onScriptInitalized() {}
		virtual void onFree() {}
		virtual void onUpdate(float dt) {}
		virtual void onFixedUpdate(float timestep) {}
		virtual void onRender2D(const SceneCamera2D& camera) {}
		virtual void onRenderUI() {}
		virtual void onInspectWindow() {}
		virtual void onInspectSettings() {}
	protected:
		void inspectSettings(Entity& selected);
		void inspectEntity(Entity& selected);
		void inspectViewport(Entity& selected);
	};
}