#include "iScene.hpp"
#include "lib/utils/yaml_common.hpp"

namespace lib
{
	void iScene::SaveToFile(const std::string& filepath)
	{
		YAML::Emitter out;
		SaveToOutput(out);
		SaveYamlFile(filepath, out);
		TraceLog(LOG_INFO, "Successfuly saved scene file: %s.", filepath.c_str());
	}

	void iScene::SaveToOutput(YAML::Emitter& out)
	{
		out << YAML::BeginMap
			<< YAML::Key << "settings" << YAML::Flow << YAML::Value
			<< YAML::BeginMap
				<< YAML::Key << "name" << YAML::Value << settings.name
				<< YAML::Key << "config" << YAML::Value << settings.configPath
				<< YAML::Key << "script" << YAML::Value << settings.scriptPath
			<< YAML::EndMap;
		
		this->onDataSave(out);

		out << YAML::Key << "world" << YAML::Value
			<< YAML::BeginSeq;
		auto view = world.view<entt::entity>();

		for (auto it = view.rbegin(); it != view.rend(); ++it) {
			Entity e(*it, world);
			if (e.hasParent() || !e.isSerializable())
				continue;
			e.Serialize(out);
		}
		out << YAML::EndSeq;
	}

	void iScene::LoadFromFile(const std::string& filepath)
	{
		std::string og_filepath = filepath;
		YAML::Node root = LoadYamlFile(filepath);
		bool isRootNull = root.IsNull();

		if (!isRootNull)
		{
			LoadFromOutput(root);
			TraceLog(LOG_INFO, "Successfuly loaded scene file: %s.", og_filepath.c_str());
		}
		else
		{
			TraceLog(LOG_ERROR, "Could not load scene file: %s.", filepath);
		}
	}

	void iScene::LoadFromOutput(const YAML::Node& root)
	{
		if (auto node = root["settings"])
		{
			readYamlValue(node["name"], &settings.name);
			readYamlValue(node["config"], &settings.configPath);
			readYamlValue(node["script"], &settings.scriptPath);
		}

		this->onDataLoad(root);


		for (auto node : root["world"])
		{
			Entity e = Entity::Create(world, "no-name");
			e.Deserialize(node);
		}
	}

}