#include "lib/components/transform2d.hpp"
#include "lib/scripting/bindings/entity_extensions.hpp"
#include <imgui.h>


namespace lib
{
	void Transform2D::serialize(YAML::Emitter& out)
	{
        out << YAML::Flow << YAML::BeginMap
            << YAML::Key << "p" << position
            << YAML::Key << "s" << size
            << YAML::Key << "o" << origin
            << YAML::Key << "a" << orientation
            << YAML::EndMap;
	}
	void Transform2D::deserialize(const YAML::Node& node)
	{
        readYamlValue(node["p"], &position);
        readYamlValue(node["s"], &size);
        readYamlValue(node["o"], &origin);
        readYamlValue(node["a"], &orientation);
	}
	bool Transform2D::inspect()
	{
        bool moved =    ImGui::DragFloat2("position", &position.x, 0.1f);
        bool resized =  ImGui::DragFloat2("size", &size.x, 0.1f);
                        ImGui::SliderFloat2("origin", &origin.x, -1, 1);
        bool rotated =  ImGui::DragFloat("orientation", &orientation, 0.1f);

        return moved || resized || rotated;
	}
	void Transform2D::ScriptBind(sol::state& lua)
	{
        namespace help = scripting::bind;

        help::register_meta_component<Transform2D>();

        lua.new_usertype<Transform2D>("Transform2D"
            , "type_id", &entt::type_hash<Transform2D>::value
            , sol::call_constructor
            , sol::factories(
                []() {return Transform2D(); },
                [](float x, float y) {return Transform2D(x, y); }
            )
            , "position", &Transform2D::position
            , "size", &Transform2D::size
            , "angle", &Transform2D::orientation
            , "orientation", &Transform2D::orientation
            , "getRectangle", &Transform2D::getRectangle
            , "getAnchoredRectangle", &Transform2D::getAnchoredRectangle
            , "getOrigin", &Transform2D::getOrigin
            , "getHeading", &Transform2D::getHeading
            //, "GetMatrix", &Transform2D::GetMatrix
        );
	}
}