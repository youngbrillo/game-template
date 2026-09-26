#pragma once
#include "lib/core/entity.hpp"
#include <raylib.h>

namespace lib
{
	struct SceneSettings
	{
		std::string name;
		std::string configPath;
		std::string scriptPath;
	};

	struct SceneViewport
	{
		int  width = 1280;
		int  height = 720;
		bool can_draw_to_target = true;
		bool can_draw_to_screen = true;
		RenderTexture2D target = { 0 };
		Color clearColor = DARKGRAY;

		inline bool isValid() const {
			return IsRenderTextureValid(target);
		}

		inline void init()
		{
			free();
			target = LoadRenderTexture(width, height);
		}

		inline void free()
		{
			if (isValid())
			{
				UnloadRenderTexture(target);
				target = RenderTexture2D{ 0 };
			}
		}
		inline void resize(int p_width, int p_height)
		{
			width = p_width;
			height = p_height;

			if (target.texture.width != width || target.texture.height != height)
			{
				init();
			}
		}
		inline void begin()
		{
			BeginTextureMode(target);
			ClearBackground(clearColor);
		}

		inline void end()
		{
			EndTextureMode();
		}

		inline static Rectangle GetCenterScreenRect_v2(Vector2 resolution, float scale) {
			return Rectangle{
				(GetScreenWidth() - (resolution.x * scale)) * 0.5f,
				(GetScreenHeight() - (resolution.y * scale)) * 0.5f,
				resolution.x * scale,
				resolution.y * scale,
			};
		}

		inline void render() const
		{
			const float v_scale = fminf((float)GetScreenWidth() / (float)width, (float)GetScreenHeight() / (float)height);
			//draw viewport
			Rectangle destination = GetCenterScreenRect_v2(Vector2{ (float)width, (float)height }, v_scale);

			renderEx(destination);
		}

		inline void renderEx(const Rectangle& destination) const
		{
			DrawTexturePro(
				target.texture,
				Rectangle{ 0, 0, target.texture.width * 1.0f, target.texture.height * -1.0f },
				destination,
				Vector2{ 0.0f,0.0f }, 0.0f, WHITE
			);
		}
	};

	class iScene
	{
	public:
		SceneSettings settings;
		entt::registry world;
	public: 
		iScene(SceneSettings p_settings)
			: settings(p_settings){}

		virtual ~iScene() {}

		virtual void init() = 0;
		virtual void free() = 0;
		virtual void update(float dt) = 0;
		virtual void fixedUpdate(float timestep) = 0;
		virtual void render() = 0;
		virtual void inspect() = 0;

		void SaveToFile(const std::string& filepath);
		void SaveToOutput(YAML::Emitter& out);

		void LoadFromFile(const std::string& filepath);
		void LoadFromOutput(const YAML::Node& root);
	protected:
		virtual void onDataSave(YAML::Emitter& out) {}
		virtual void onDataLoad(const YAML::Node& root) {}
	};
}