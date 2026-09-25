#pragma once 
#include "lib/core/iScene.hpp"
#include "lib/components/sceneCamera3d.hpp"
#include "lib/scripting/luaScript.hpp"

namespace lib
{
	class Scene3D : public iScene
	{
	public:
		SceneCamera3D default_camera;
		LuaScript mainScript;
	public:
		Scene3D(SceneSettings p_settings);
		virtual ~Scene3D();

		virtual void init();
		virtual void free();
		virtual void update(float dt);
		virtual void fixedUpdate(float timestep);
		virtual void render();

		const SceneCamera3D& getSceneCamera() const;
	protected:
		virtual void onInit() {};
		virtual void onFree() {};
		virtual void onUpdate(float dt) {};
		virtual void onFixedUpdate(float timestep) {};
		virtual void onRender() {};
	};
}