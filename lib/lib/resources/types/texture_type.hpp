#pragma once
#include "lib/resources/resource_types.hpp"

namespace lib
{
	class TextureResource : public SceneResource
	{
	public:
		Texture2D data = { 0 };
		int filter = TextureFilter::TEXTURE_FILTER_POINT;
		int wrap   = TextureWrap::TEXTURE_WRAP_REPEAT;

		virtual void load() override;
		virtual void free() override;
		virtual bool isValid() const override;
		void setFilter(int filter);
		void setWrap(int wrap);
	protected:
		virtual void onRead(const YAML::Node& node) override;
		virtual void onWrite(YAML::Emitter& out) const override;
		virtual void onInspectWindow() override;
	};
}