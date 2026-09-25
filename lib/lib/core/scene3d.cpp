#include "lib/core/scene3d.hpp"

namespace lib
{
	Scene3D::Scene3D(SceneSettings p_settings)
		: iScene(p_settings)
	{
	}
	Scene3D::~Scene3D()
	{
	}
	void Scene3D::init()
	{
	}
	void Scene3D::free()
	{
	}
	void Scene3D::update(float dt)
	{
	}
	void Scene3D::fixedUpdate(float timestep)
	{
	}
	void Scene3D::render()
	{
	}
	const SceneCamera3D& Scene3D::getSceneCamera() const
	{
		// TODO: insert return statement here
		for (auto&& [id, cam] : world.view<SceneCamera3D>(entt::exclude<components::DisabledTag>).each())
		{
			return cam;
		}

		return default_camera;
	}
}