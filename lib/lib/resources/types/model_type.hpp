#pragma once
#include "lib/resources/resource_types.hpp"

namespace lib
{
	class ModelResource : public SceneResource
	{
	public:
		Model data = { 0 };

		ModelResource() = default;
		ModelResource(const ModelResource& res) = default;
		ModelResource(std::string p_name, UUID p_id) : SceneResource(p_name, p_id, SceneResourceType_Model) {}


		virtual void load() override;
		virtual void free() override;
		virtual bool isValid() const override;
	protected:
		virtual void onRead(const YAML::Node& node) override;
		virtual void onWrite(YAML::Emitter& out) const override;
		virtual void onInspectWindow() override;
	};
}