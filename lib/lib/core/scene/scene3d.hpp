#pragma once 
#include "iAppScene.hpp"
#include "lib/components/3d/sceneCamera3d.hpp"
#include "lib/scripting/luaScript.hpp"
#include <box3d/box3d.h>

namespace lib
{

	class Scene3D : public iAppScene
	{
	public:
		SceneCamera3D default_camera;
		b3WorldId worldId;

	public:
		Scene3D(SceneSettings p_settings);
		virtual ~Scene3D();

		virtual void init();
		virtual void free();
		virtual void update(float dt);
		virtual void fixedUpdate(float timestep);
		virtual void render();
		virtual void renderScene();

		const SceneCamera3D& getSceneCamera() const;
	protected:
		virtual void onInspectViewportTop(Entity& selected) override;
		virtual void onInspectViewportBottom(float* viewportPos, float* viewportSize, Entity& selected) override;
	};
}