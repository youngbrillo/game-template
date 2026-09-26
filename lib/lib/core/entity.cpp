#include "lib/core/entity.hpp"
#include "lib/utils/yaml_common.hpp"
#include <unordered_set>	//req. for make_unique_name
#include <regex>			//req. for make_unique_name
#include <lib/components/components3d.hpp>
#include <lib/utils/editor_utils.hpp>
#include <imgui_stdlib.h>

namespace lib
{
	static SerializeEntityCallback writeEntityCallback = nullptr;
	static DeserializeEntityCallback readEntityCallback = nullptr;


	void SetSerializeEntityCallback(SerializeEntityCallback callback)
	{
		writeEntityCallback = callback;
	}
	void SetDeserializeEntityCallback(DeserializeEntityCallback callback)
	{
		readEntityCallback = callback;
	}

	void Entity::destroy()
	{
		addOrReplace<components::DeleteTag>();
		while (getFirstChild())
		{
			Entity child = getFirstChild();
			child.destroy();
		}
		removeParent();
	}
	std::string Entity::getName()
	{
		if (auto* nameTag = tryGet<components::NameTag>(); nameTag)
		{
			return nameTag->name;
		}
		return "Unnamed";
	}
	UUID Entity::getUUID()
	{
		return get<components::IDTag>().tag;
	}
	bool Entity::isEnabled()
	{
		return has<components::DisabledTag>() == false;
	}
	bool Entity::isVisible()
	{
		return has<components::HiddenTag>() == false;
	}
	bool Entity::isSerializable()
	{
		return has<components::DoNotSerializeTag>() == false;
	}
	void Entity::enable()
	{
		remove<components::DisabledTag>();
	}
	void Entity::disable()
	{
		addOrReplace<components::DisabledTag>();
	}
	void Entity::hide()
	{
		addOrReplace<components::HiddenTag>();
	}
	void Entity::show()
	{
		remove<components::HiddenTag>();
	}
	void Entity::disableSerialization()
	{
		addOrReplace<components::DoNotSerializeTag>();
	}
	void Entity::enableSerialization()
	{
		remove<components::DoNotSerializeTag>();
	}

	static std::string make_unique_name(const std::string& name, const std::unordered_set<std::string>& existing_names)
	{
		if (!existing_names.contains(name))
			return name;

		// Strip an existing " (number)" suffix.
		static const std::regex suffixRegex(R"(^(.*) \((\d+)\)$)");

		std::smatch match;
		std::string baseName = name;
		int startingIndex = 1;

		if (std::regex_match(name, match, suffixRegex))
			baseName = match[1].str();
		if (match.size() >= 3)
		{
			startingIndex = std::stoi(match[2].str());
		}


		// Try(1), (2), (3), ...
		for (int i = startingIndex; ; ++i)
		{
			std::string candidate = baseName + " (" + std::to_string(i) + ")";

			if (!existing_names.contains(candidate))
				return candidate;
		}

	}

	Entity Entity::duplicate()
	{
		Entity e(_world->create(), _world);
		auto& name= e.add<components::NameTag>();
		auto& id = e.add<components::IDTag>();

		YAML::Emitter output;
		this->Serialize(output);
		std::string input = output.c_str();

		YAML::Node root = YAML::Load(input);

		e.Deserialize(root);

		if (hasParent())
		{
			e.addParent(getParent());
		}

		std::unordered_set<std::string> existing_names = {
			name.name
		};

		name.name = make_unique_name(name.name, existing_names);



		return e;
	}

	template<typename T>
	static void WriteComponent(YAML::Emitter& out, Entity* entity, const char* label)
	{
		if (auto* component = entity->tryGet<T>())
		{
			out << YAML::Key << label << YAML::Value << *component;
		}

	}
	template<typename T>
	static bool ReadComponent(const YAML::Node& node, Entity* entity)
	{
		bool found = false;
		if (node && entity)
		{
			T component;
			ReadYamlValue(node, &component);
			entity->add<T>(component);
			found = true;
		}

		return false;
	}

	void Entity::Serialize(YAML::Emitter& out)
	{

		auto& name = get<components::NameTag>();
		auto& id   = get<components::IDTag>();

		out << YAML::BeginMap
			<< YAML::Key << "entity" << YAML::Value
			<< YAML::Flow << YAML::BeginMap
			<< YAML::Key << "name" << YAML::Value << name.name
			<< YAML::Key << "id" << YAML::Value << id.tag;
		if (this->isVisible() == false) out << YAML::Key << "visible" << YAML::Value << false;
		if (this->isEnabled() == false) out << YAML::Key << "enabled" << YAML::Value << false;

		if (auto* c = tryGet<Transform3D>())
		{
			out << YAML::Key << "Transform3D" << YAML::Value;
			c->Serialize(out);
		}
		if (auto* c = tryGet<SceneCamera3D>()){
			out << YAML::Key << "SceneCamera3D" << YAML::Value;
			c->write(out);
		}

		if (auto* c = tryGet<StaticMesh>())
		{
			out << YAML::Key << "StaticMesh" << YAML::Value;
			c->write(out);
		}
		if (auto* c = tryGet<Rigidbody3D>()) {
			out << YAML::Key << "Rigidbody3D" << YAML::Value;
			c->write(out);
		}
		if (auto* c = tryGet<BoxCollider3D>()) {
			out << YAML::Key << "BoxCollider3D" << YAML::Value;
			c->write(out);
		}
		if (auto* c = tryGet<SphereCollider3D>()) {
			out << YAML::Key << "SphereCollider3D" << YAML::Value;
			c->write(out);
		}


		if (writeEntityCallback) writeEntityCallback(out, *this);

		if (auto child = getFirstChild())
		{
			out << YAML::Key << "children" << YAML::Value
				<< YAML::BeginSeq;
			while (child)
			{
				child.Serialize(out);
				child = child.getNextSibling();
			}
			out << YAML::EndSeq;
		}

		out << YAML::EndMap;

	}
	void Entity::Deserialize(const YAML::Node& node)
	{
		if (auto iNode = node["entity"])
		{
			auto& info = get<components::NameTag>();
			auto& id = get<components::IDTag>();
			bool isVisible = true;
			bool isEnabled= true;
			readYamlValue(iNode["name"], &info.name);
			readYamlValue(iNode["id"], &id.tag);
			if (readYamlValue(iNode["visible"], &isVisible) && isVisible == false)
			{
				hide();
			}
			if (readYamlValue(iNode["enabled"], &isEnabled) && isEnabled == false)
			{
				disable();
			}

			if (auto n = node["Transform3D"])
			{
				auto& c = add<Transform3D>();
				c.Deserialize(n);
			}
			if (auto n = node["SceneCamera3D"]) {
				auto& c = add<SceneCamera3D>();
				c.read(n);
			}
			if (auto n = node["StaticMesh"]) {
				auto& c = add<StaticMesh>();
				c.read(n);
			}
			if (auto n = node["Rigidbody3D"]) {
				auto& c = add<Rigidbody3D>();
				c.read(n);
			}
			if (auto n = node["BoxCollider3D"]) {
				auto& c = add<BoxCollider3D>();
				c.read(n);
			}
			if (auto n = node["SphereCollider3D"]) {
				auto& c = add<SphereCollider3D>();
				c.read(n);
			}

			if (readEntityCallback) readEntityCallback(node, *this);

			if (auto children = node["children"]) {
				for (auto child : children)
				{
					Entity c = createChild("nameless-child");
					c.Deserialize(child);
				}
			}
		}
	}
	void Entity::inspect()
	{
		auto& name_tag = get<components::NameTag>();
		ImGui::InputText("name", &name_tag.name);

	}
	Entity Entity::createChild(std::string name)
	{
		Entity e = Entity::Create(*_world, name);
		addChild(e);
		return e;
	}
	void Entity::addParent(Entity parent)
	{
		using namespace components;

		Relationship& r = get<Relationship>();
		if (r.parent == parent)
			return;
		if (r.parent != entt::null)
			this->removeParent(); //handles all delinking
		parent.addChild(*this);
	}
	void Entity::removeParent()
	{
		using namespace components;
		Relationship& r = get<Relationship>();
		if (r.parent == entt::null)
			return;

		Entity parent(r.parent, _world);
		parent.removeChild(*this); //handles sibling delinking
		r.parent = entt::null;
	}
	void Entity::addChild(Entity child)
	{
		using namespace components;
		Relationship& r = get<Relationship>();
		if (isChild(child) || child == *this)
			return; //don't add duplicate children! and you cannot be a child of yourself!

		Relationship& childR = child.get<Relationship>();
		if (child.getParent())
			child.removeParent();

		childR.parent = _handle;

		entt::entity it = r.firstChild;
		if (it == entt::null) //no children
		{
			r.firstChild = child;
		}
		else //not the first child, add it to the end of the linked list
		{
			while (it != entt::null)
			{
				Entity sib(it, _world);
				auto& nr = sib.get<Relationship>();
				if (nr.nextSib == entt::null)
				{
					nr.nextSib = child;
					childR.prevSib = sib;
					it = entt::null;
				}
				else
					it = nr.nextSib;
			}
		}
	}
	void Entity::removeChild(Entity child)
	{
		using namespace components;
		Relationship& pr = get<Relationship>();

		if (!isChild(child))
			return;

		Relationship& r = child.get<Relationship>();

		if (pr.firstChild == child)
		{
			pr.firstChild = r.nextSib;
		}

		if (r.prevSib != entt::null)
		{
			Relationship& pr = child.getPrevSibling().get<Relationship>();
			pr.nextSib = r.nextSib;
		}
		if (r.nextSib != entt::null)
		{
			Relationship& nr = child.getNextSibling().get<Relationship>();
			nr.prevSib = r.prevSib;
		}

		r.prevSib = entt::null;
		r.nextSib = entt::null;
		r.parent = entt::null;
	}
	void Entity::removeAllChildren()
	{
		for (Entity i = getFirstChild(); i.isValid(); getFirstChild())
		{
			removeChild(i);
		}
	}
	Entity Entity::getFirstChild()
	{
		return Entity(get<components::Relationship>().firstChild, _world);
	}
	Entity Entity::getNextSibling()
	{
		return Entity(get<components::Relationship>().nextSib, _world);
	}
	Entity Entity::getPrevSibling()
	{
		return Entity(get<components::Relationship>().prevSib, _world);
	}
	Entity Entity::getParent()
	{
		return Entity(get<components::Relationship>().parent, _world);
	}
	Entity Entity::getChildNamed(std::string name)
	{
		Entity it = getFirstChild();
		while (it)
		{
			if (it.getName() == name)
				return it;
			it = it.getNextSibling();
		}
		return Entity();
	}
	bool Entity::isChild(Entity child)
	{
		Entity it = getFirstChild();
		while (it)
		{
			if (it == child)
				return true;
			it = it.getNextSibling();
		}
		return false;
	}
	bool Entity::hasParent()
	{
		return get<components::Relationship>().parent != entt::null;

	}
	bool Entity::isDecendantOf(Entity ancestor)
	{
		Entity family = getParent();
		while (family)
		{
			if (family == ancestor)
				return true;

			family = family.getParent();
		}
		return false;
	}

	uint32_t Entity::getNumberOfChildren()
	{
		uint32_t count = 0;

		for (Entity i = getFirstChild(); i.isValid(); i.getNextSibling())
		{
			count++;
		}
		return count;
	}

	Entity Entity::Create(entt::registry& world, std::string name)
	{
		Entity e(world.create(), world);
			e.add<components::NameTag>().name = name;
			e.add<components::IDTag>().tag = UUID();
			e.add<components::Relationship>();

		return e;
	}

	static uint32_t current_dragged_entity_handle;


	void Entity::DragSource(const char* label, Entity e)
	{
		if (ImGui::BeginDragDropSource()) {
			current_dragged_entity_handle = e.getHandleInt32();
			ImGui::SetDragDropPayload("ENTITY_PAYLOAD", &current_dragged_entity_handle, sizeof(uint32_t));
			ImGui::Text("%s '%s'", label, e.getName().c_str());
			ImGui::EndDragDropSource();
		}
	}
	bool Entity::DropTarget(Entity& e, entt::registry& world)
	{

		bool wasDropped = false;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_PAYLOAD"))
			{
				uint32_t payload_entity = *(uint32_t*)payload->Data;
				e = Entity(entt::entity{ payload_entity }, world);
				wasDropped = true;
			}

			ImGui::EndDragDropTarget();
		}

		return wasDropped;
	}
}