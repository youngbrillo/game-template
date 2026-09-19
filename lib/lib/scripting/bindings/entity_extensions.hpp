#pragma once
#include "lib/core/entity.hpp"
#include <sol/sol.hpp>
#include <raylib.h>			//include this for the trace log

namespace lib::scripting::bind
{
	inline [[nodiscard]] entt::id_type get_type_id(const sol::table& obj) {
		const auto f = obj["type_id"].get<sol::function>();
		assert(f.valid() && "type_id not exposed to lua!");
		return f.valid() ? f().get<entt::id_type>() : -1;
	}

	template <typename T> inline [[nodiscard]] entt::id_type deduce_type(T&& obj) {
		switch (obj.get_type()) {
			// in lua: registry:has(e, Transform.type_id())
		case sol::type::number:
			return obj.template as<entt::id_type>();
			// in lua: registry:has(e, Transform)
		case sol::type::table:
			return get_type_id(obj);
		}
		assert(false);
		return -1;
	}

	// @see
	// https://github.com/skypjack/entt/wiki/Crash-Course:-runtime-reflection-system
	template <typename... Args>
	inline auto invoke_meta_func(entt::meta_type meta_type,
		entt::id_type function_id, Args &&...args) {
		if (!meta_type) {
			TraceLog(LOG_WARNING, "[invoke_meta_func]\tMeta Type (%d) is not valid. Make sure you are calling 'register_meta_component' for this Meta Type.", function_id);
		}
		else {
			if (auto&& meta_function = meta_type.func(function_id); meta_function)
				return meta_function.invoke({}, std::forward<Args>(args)...);
		}
		return entt::meta_any{};
	}

	template <typename... Args>
	inline auto invoke_meta_func(entt::id_type type_id, entt::id_type function_id,
		Args &&...args) {
		return invoke_meta_func(entt::resolve(type_id), function_id,
			std::forward<Args>(args)...);
	}

	namespace meta
	{
		template <typename Component>
		inline auto emplace_component(Entity* entity, const sol::table& instance, sol::this_state s)
		{
			assert(entity);
			bool valid = instance.valid();
			auto& comp = entity->add<Component>(
				valid ? std::move(instance.as<Component&&>()) : Component{});

			return sol::make_reference(s, std::ref(comp));
		}

		template <typename Component>
		inline auto add_or_replace_component(Entity* entity, const sol::table& instance, sol::this_state s) {
			assert(entity);
			bool valid = instance.valid();
			auto& comp = entity->addOrReplace<Component>(
				valid ? std::move(instance.as<Component&&>()) : Component{});

			return sol::make_reference(s, std::ref(comp));
		}

		template <typename Component>
		inline auto get_component(Entity* entity, sol::this_state s) {
			assert(entity);
			auto& comp = entity->get<Component>();
			return sol::make_reference(s, std::ref(comp));
		}


		template <typename Component>
		inline auto get_or_add_component(Entity* entity, sol::this_state s) {
			assert(entity);
			auto& comp = entity->get<Component>();
			return sol::make_reference(s, std::ref(comp));
		}

		template <typename Component>
		inline Component* try_get_component(Entity* entity, const sol::table& instance, sol::this_state s) {
			assert(entity);
			auto* comp = entity->tryGet<Component>();
			return comp;
		}

		template <typename Component>
		inline bool has_component(Entity* entity) {
			assert(entity);
			return entity->has<Component>();
		}

		template <typename Component>
		inline auto remove_component(Entity* entity) {
			assert(entity);
			return entity->remove<Component>();
		}
	}

	template <typename Component> inline void register_meta_component() {
		using namespace entt::literals;

		entt::meta_factory<Component>()
			//.template func<&meta::is_valid<Component>>("valid"_hs)
			.template func<&meta::emplace_component<Component>>("add"_hs)
			.template func<&meta::get_component<Component>>("get"_hs)
			.template func<&meta::has_component<Component>>("has"_hs)
			.template func<&meta::remove_component<Component>>("remove"_hs);
	}

	inline auto collect_types(const sol::variadic_args& va) {
		std::set<entt::id_type> types;
		std::transform(va.cbegin(), va.cend(), std::inserter(types, types.begin()),
			[](const auto& obj) { return deduce_type(obj); });
		return types;
	}


	inline sol::object entity_bind_emplace(Entity& self, const sol::table& comp, sol::this_state s)
	{
		using namespace entt::literals;
		if (!comp.valid()) {
			return sol::lua_nil_t{};
		}

		entt::id_type typeId = get_type_id(comp);
		const auto maybe_any = invoke_meta_func(typeId, "add"_hs,
			&self, comp, s);
		return maybe_any ? maybe_any.cast<sol::reference>() : sol::lua_nil_t{};
	}

	inline sol::object entity_bind_get(Entity& self, const sol::object& type_or_id, sol::this_state s)
	{
		using namespace entt::literals;
		const auto maybe_any = invoke_meta_func(deduce_type(type_or_id), "get"_hs,
			&self, s);
		return maybe_any ? maybe_any.cast<sol::reference>() : sol::lua_nil_t{};
	}
	inline bool entity_bind_has(Entity& self, const sol::object& type_or_id)
	{
		using namespace entt::literals;
		const auto maybe_any = invoke_meta_func(deduce_type(type_or_id), "has"_hs, &self);
		return maybe_any ? maybe_any.cast<bool>() : false;
	}
	inline bool entity_bind_remove(Entity& self, const sol::object& type_or_id)
	{
		using namespace entt::literals;
		const auto maybe_any = invoke_meta_func(deduce_type(type_or_id), "remove"_hs, &self);
		return maybe_any ? maybe_any.cast<bool>() : false;
	}

}