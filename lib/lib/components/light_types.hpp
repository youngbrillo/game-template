#pragma once
#include "transform3d.hpp"
#include "sceneCamera3d.hpp"
#include <lib/core/uuid.hpp>

namespace lib
{
    Shader LoadBasicLightingShader();

    struct Light3D
    {
		enum LIGHT_TYPE
		{
			LIGHT_TYPE_DIRECTIONAL,
			LIGHT_TYPE_POINT
		};
		Shader shader = { 0 };
		int type = LIGHT_TYPE_DIRECTIONAL;
		bool enabled = true;
		Color color = WHITE;
		float attenuation = 1.0f;

		// Shader uniforms
		struct
		{
			int enabled = -1;
			int type = -1;
			int position = -1;
			int target = -1;
			int color = -1;
			int attenuation = -1;
		} uniforms;
		int index = 0; //must be set by hand 

		inline bool isValid() const { return IsShaderValid(shader); }
		void init(Shader p_shader, Vector3 p_position, Vector3 p_target);
		void free();
		void SetShader(Shader p_shader);
		void update(Vector3 p_position, Vector3 p_target);

		static void SetLightFromTransform(const Transform3D& transform, Light3D& light);
		static void SetLightFromCamera(const SceneCamera3D& camera, Light3D& light);


		void inspect();
		void write(YAML::Emitter& out);
		void read(const YAML::Node& node);
		static void Bind(sol::state& lua);
    };
    
    struct Light3DManager
    {
		UUID id = 0;
        Shader shader = {0};
		bool ownsShader = false;
		Vector4 ambient = ColorNormalize(LIGHTGRAY);
		int ambientLoc = -1;

		inline bool isValid() const { return IsShaderValid(shader); }
		void init(Shader p_shader);
        void free();
		void update(Vector3 p_camera_position);
		void setAmbientColor(Color p_color);
		void setAmbientColorEx(Vector4 p_color);
		void inspect();
		void write(YAML::Emitter& out);
		void read(const YAML::Node& node);
		static void Bind(sol::state& lua);
    };

}