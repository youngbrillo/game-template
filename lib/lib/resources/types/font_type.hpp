#pragma once
#include "lib/resources/resource_types.hpp"

namespace lib
{
	class FontResource : public SceneResource
	{
	public:
		Font data = { 0 };
		float size = 24;
		int filter = TextureFilter::TEXTURE_FILTER_POINT;

		FontResource() = default;
		FontResource(const FontResource& res) = default;
		FontResource(std::string p_name, UUID p_id) : SceneResource(p_name, p_id, SceneResourceType_Font) {}

		virtual void load() override;
		virtual void free() override;
		virtual bool isValid() const override;
		virtual void setFilter(int p_filter);
		virtual void setSize(int p_size);

	protected:
		virtual void onRead(const YAML::Node& node) override;
		virtual void onWrite(YAML::Emitter& out) const override;
		virtual void onInspectWindow() override;
	};
}