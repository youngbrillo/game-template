#include "light_types.hpp"
#include "lib/utils/yaml_common.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include <imgui.h>

namespace lib
{

	namespace internalShaders
	{
		const char* lighting_shader_vs_src =
			"#version 330\n"
			"\n"
			"	// Input vertex attributes\n"
			"	in vec3 vertexPosition;\n"
			"in vec2 vertexTexCoord;\n"
			"in vec3 vertexNormal;\n"
			"in vec4 vertexColor;\n"
			"\n"
			"// Input uniform values\n"
			"uniform mat4 mvp;\n"
			"uniform mat4 matModel;\n"
			"uniform mat4 matNormal;\n"
			"\n"
			"// Output vertex attributes (to fragment shader)\n"
			"out vec3 fragPosition;\n"
			"out vec2 fragTexCoord;\n"
			"out vec4 fragColor;\n"
			"out vec3 fragNormal;\n"
			"\n"
			"// NOTE: Add your custom variables here\n"
			"\n"
			"void main()\n"
			"{\n"
			"	// Send vertex attributes to fragment shader\n"
			"	fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));\n"
			"	fragTexCoord = vertexTexCoord;\n"
			"	fragColor = vertexColor;\n"
			"	fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));\n"
			"\n"
			"	// Calculate final vertex position\n"
			"	gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
			"}\n"
			;

		const char* lighting_shader_fs_src =
			"#version 330\n"
			"\n"
			"// Input vertex attributes (from vertex shader)\n"
			"in vec3 fragPosition;\n"
			"in vec2 fragTexCoord;\n"
			"in vec4 fragColor;\n"
			"in vec3 fragNormal;\n"
			"\n"
			"// Input uniform values\n"
			"uniform sampler2D texture0;\n"
			"uniform vec4 colDiffuse;\n"
			"\n"
			"// Output fragment color\n"
			"out vec4 finalColor;\n"
			"\n"
			"// NOTE: Add your custom variables here\n"
			"\n"
			"#define     MAX_LIGHTS              4\n"
			"#define     LIGHT_DIRECTIONAL       0\n"
			"#define     LIGHT_POINT             1\n"
			"\n"
			"struct Light {\n"
			"	int enabled;\n"
			"	int type;\n"
			"	vec3 position;\n"
			"	vec3 target;\n"
			"	vec4 color;\n"
			"};\n"
			"\n"
			"// Input lighting values\n"
			"uniform Light lights[MAX_LIGHTS];\n"
			"uniform vec4 ambient;\n"
			"uniform vec3 viewPos;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	// Texel color fetching from texture sampler\n"
			"	vec4 texelColor = texture(texture0, fragTexCoord);\n"
			"	vec3 lightDot = vec3(0.0);\n"
			"	vec3 normal = normalize(fragNormal);\n"
			"	vec3 viewD = normalize(viewPos - fragPosition);\n"
			"	vec3 specular = vec3(0.0);\n"
			"\n"
			"	vec4 tint = colDiffuse * fragColor;\n"
			"\n"
			"	// NOTE: Implement here your fragment shader code\n"
			"\n"
			"	for (int i = 0; i < MAX_LIGHTS; i++)\n"
			"	{\n"
			"		if (lights[i].enabled == 1)\n"
			"		{\n"
			"			vec3 light = vec3(0.0);\n"
			"\n"
			"			if (lights[i].type == LIGHT_DIRECTIONAL)\n"
			"			{\n"
			"				light = -normalize(lights[i].target - lights[i].position);\n"
			"			}\n"
			"\n"
			"			if (lights[i].type == LIGHT_POINT)\n"
			"			{\n"
			"				light = normalize(lights[i].position - fragPosition);\n"
			"			}\n"
			"\n"
			"			float NdotL = max(dot(normal, light), 0.0);\n"
			"			lightDot += lights[i].color.rgb * NdotL;\n"
			"\n"
			"			float specCo = 0.0;\n"
			"			if (NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-(light), normal))), 16.0); // 16 refers to shine\n"
			"			specular += specCo;\n"
			"		}\n"
			"	}\n"
			"\n"
			"	finalColor = (texelColor * ((tint + vec4(specular, 1.0)) * vec4(lightDot, 1.0)));\n"
			"	finalColor += texelColor * (ambient / 10.0) * tint;\n"
			"\n"
			"	// Gamma correction\n"
			"	finalColor = pow(finalColor, vec4(1.0 / 2.2));\n"
			"}\n"
			;
	}

    Shader LoadBasicLightingShader()
    {
		Shader shader = LoadShaderFromMemory(lib::internalShaders::lighting_shader_vs_src, internalShaders::lighting_shader_fs_src);
		return shader;
    }
    void Light3D::init(Shader p_shader, Vector3 p_position, Vector3 p_target)
    {
        SetShader(p_shader);
        update(p_position, p_target);
    }
	void Light3D::free()
	{
		this->enabled = false;
		this->update(Vector3Zeros, Vector3Zeros);
	}
    void Light3D::SetShader(Shader p_shader)
    {
        if (IsShaderValid(p_shader))
        {
            // NOTE: Lighting shader naming must be the provided ones
            this->uniforms.enabled = GetShaderLocation(p_shader, TextFormat("lights[%i].enabled", index));
            this->uniforms.type = GetShaderLocation(p_shader, TextFormat("lights[%i].type", index));
            this->uniforms.position = GetShaderLocation(p_shader, TextFormat("lights[%i].position", index));
            this->uniforms.target = GetShaderLocation(p_shader, TextFormat("lights[%i].target", index));
            this->uniforms.color = GetShaderLocation(p_shader, TextFormat("lights[%i].color", index));
        }
		shader = p_shader;
        TraceLog(LOG_INFO, "[LightComponent]:\t Initialized Light Component at index: %d.", index);
    }
    void Light3D::update(Vector3 p_position, Vector3 p_target)
    {
        if (isValid())
        {
            // Send to shader light enabled state and type
            int en_id = this->enabled ? 1 : 0;
            SetShaderValue(shader, this->uniforms.enabled, &en_id, SHADER_UNIFORM_INT);
            SetShaderValue(shader, this->uniforms.type, &this->type, SHADER_UNIFORM_INT);

            // Send to shader light position values
            SetShaderValue(shader, this->uniforms.position, &p_position.x, SHADER_UNIFORM_VEC3);

            // Send to shader light target position values
            //target = transform.position + transform.Front();
            SetShaderValue(shader, this->uniforms.target, &p_target.x, SHADER_UNIFORM_VEC3);

            // Send to shader light color values
            Vector4 vcolor = ColorNormalize(color);
            SetShaderValue(shader, uniforms.color, &vcolor.x, SHADER_UNIFORM_VEC4);
        }
    }
	void Light3D::SetLightFromTransform(const Transform3D& transform, Light3D& light)
	{
		light.update(transform.position, transform.position + transform.Front());
	}
	void Light3D::SetLightFromCamera(const SceneCamera3D& camera, Light3D& light)
	{
		light.update(camera.camera.position, camera.camera.target);
	}

	static void inspect_shader_loc(const char* label, int location)
	{
		static ImVec4 invalidCol = { 0.5f, 0.5f, 0.5f, 1.0f };
		static ImVec4 validCol = { 0.0f, 1.f, 0.3f, 1.0f };
		ImVec4 col = location == -1 ? invalidCol : validCol;

		ImGui::TextColored(col, "%s Loc: %d", label, location);
	}

	static bool inspect_shader_loc_w_radio(const char* label, int location, bool& ref)
	{
		bool changed = false;
		inspect_shader_loc(label, location);
		ImGui::SameLine();
		if (ImGui::RadioButton(label, ref))
		{
			ref = !ref;
			changed = true;
		}

		return changed;
	}

    void Light3D::inspect()
    {
		inspect_shader_loc_w_radio("enabled", uniforms.enabled, enabled);
		inspect_shader_loc("type", uniforms.type);

		ImGui::SameLine();
		if (ImGui::RadioButton("Directional", type == LIGHT_TYPE_DIRECTIONAL))
		{
			type = LIGHT_TYPE_DIRECTIONAL;
		}

		ImGui::SameLine();
		if (ImGui::RadioButton("Point", type == LIGHT_TYPE_POINT))
		{
			type = LIGHT_TYPE_POINT;
		}

		inspect_shader_loc("position", uniforms.position);
		inspect_shader_loc("target", uniforms.target);
		inspect_shader_loc("color", uniforms.color);
		inspect_shader_loc("attenuation", uniforms.attenuation);

		Vector4 ncolor = ColorNormalize(color);
		if (ImGui::ColorEdit4("color", &ncolor.x))
		{
			color = ColorFromNormalized(ncolor);
		}
    }
	void Light3D::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "index" << YAML::Value << index
			<< YAML::Key << "type" << YAML::Value << type
			<< YAML::Key << "enabled" << YAML::Value << enabled
			<< YAML::Key << "color" << YAML::Value << color
			<< YAML::EndMap;
	}
	void Light3D::read(const YAML::Node& node)
	{
		readYamlValue(node["index"], &index);
		readYamlValue(node["type"], &type);
		readYamlValue(node["enabled"], &enabled);
		readYamlValue(node["color"], &color);
	}
	void Light3D::Bind(sol::state& lua)
	{
	}


    void Light3DManager::init(Shader p_shader)
    {
        shader = p_shader;

        shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
        ambientLoc = GetShaderLocation(shader, "ambient");
        SetShaderValue(shader, ambientLoc, &ambient.x, SHADER_UNIFORM_VEC4);
    }
    void Light3DManager::free()
    {
		if(ownsShader)
			UnloadShader(shader);

    }
    void Light3DManager::update(Vector3 p_camera_position)
    {
		if (isValid())
		{
			SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &p_camera_position.x, SHADER_UNIFORM_VEC3);
		}
    }
    void Light3DManager::setAmbientColor(Color p_color)
    {
		Vector4 p_ambient = ColorNormalize(p_color);
		setAmbientColorEx(p_ambient);
    }
    void Light3DManager::setAmbientColorEx(Vector4 p_color)
    {
		ambient = p_color;
		if (isValid())
		{
			SetShaderValue(shader, ambientLoc, &ambient.x, SHADER_UNIFORM_VEC4);
		}
    }
    void Light3DManager::inspect()
    {

		if (ImGui::ColorEdit4("ambient", &ambient.x))
		{
			setAmbientColorEx(ambient);
		}
    }
	void Light3DManager::write(YAML::Emitter& out)
	{
		out << YAML::Flow << YAML::BeginMap
			<< YAML::Key << "id" << YAML::Value << id
			<< YAML::Key << "ownsShader" << YAML::Value << ownsShader
			<< YAML::Key << "ambient" << YAML::Value << ambient
			<< YAML::EndMap;
	}
	void Light3DManager::read(const YAML::Node& node)
	{
		readYamlValue(node["id"], &id);
		readYamlValue(node["ownsShader"], &ownsShader);
		readYamlValue(node["ambient"], &ambient);
	}
	void Light3DManager::Bind(sol::state& lua)
	{
	}
}

