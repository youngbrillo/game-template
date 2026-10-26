#pragma once
#include "iScene.hpp"
#include "lib/scripting/luaScript.hpp"

namespace lib
{
	struct SceneCamera3D;
	struct SceneCamera2D;

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

	class iAppScene : public iScene
	{
	protected:
		SceneViewport viewport;
		LuaScript mainScript;
		Entity m_selected_entity;
		bool m_show_imgui_demo = true;
		SceneInspector m_scene_inspector;
	public:
		iAppScene(SceneSettings p_settings);
		virtual ~iAppScene();

		virtual void render() override;
		virtual void renderScene() = 0;
		virtual void inspect();
	protected:
		virtual void onInit() {}
		virtual void onScriptInitalized() {}
		virtual void onFree() {}
		virtual void onUpdate(float dt) {}
		virtual void onFixedUpdate(float timestep) {}
		virtual void onRender2D(const SceneCamera2D& camera) {}
		virtual void onRender3D(const SceneCamera3D& camera) {}
		virtual void onRenderUI() {}
		virtual void onInspectWindow() {}
		virtual void onInspectSettings() {}

		void inspectSettings(Entity& selected);
		void inspectEntity(Entity& selected);
		void inspectViewport(Entity& selected);

		virtual void onInspectSettings(Entity& selected) {}
		virtual void onInspectEntity(Entity& selected) {}
		virtual void onInspectViewportTop(Entity& selected) {}
		virtual void onInspectViewportBottom(float* viewportPos, float* viewportSize, Entity& selected) {}
	};
}
