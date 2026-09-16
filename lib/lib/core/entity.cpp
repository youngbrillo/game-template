#include "lib/core/entity.hpp"

namespace lib
{
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
	Entity Entity::duplicate()
	{
		return Entity();
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
}