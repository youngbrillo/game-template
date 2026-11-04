#include "model_type.hpp"

namespace lib
{
	void ModelResource::load()
	{
		if (!path.empty())
		{
			if (path == "cube")
				data = ::LoadModelFromMesh(GenMeshCube(1, 1, 1));
			else if (path == "sphere")
				data = ::LoadModelFromMesh(GenMeshSphere(0.5f, 32, 32));
			else if (path == "plane")
				data = ::LoadModelFromMesh(GenMeshPlane(1, 1, 1, 1));
			else if (path == "torus")
				data = ::LoadModelFromMesh(GenMeshTorus(0.5f, 1.0f, 16, 32));
			else if (path == "cylinder")
				data = ::LoadModelFromMesh(GenMeshCylinder(0.5f, 1.0f, 16));
			else
				data = ::LoadModel(path.c_str());
		}
	}
	void ModelResource::free()
	{
		if (isValid())
			UnloadModel(data);

		data = { 0 };
	}
	bool ModelResource::isValid() const
	{
		return IsModelValid(data);
	}
	void ModelResource::onRead(const YAML::Node& node)
	{
	}
	void ModelResource::onWrite(YAML::Emitter& out) const
	{
	}
	void ModelResource::onInspectWindow()
	{
	}
}