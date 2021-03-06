#pragma once

#include <blueprint/typedef.h>
#include <node0/typedef.h>
#include <SM_Vector.h>

#include <string>
#include <vector>

namespace shaderlab
{

class NodeBuilder
{
public:
	static bp::NodePtr Create(std::vector<n0::SceneNodePtr>& nodes, const std::string& type,
		const sm::vec2& pos = sm::vec2(0, 0), bool is_default = false);

	static void CreateDefaultInputs(std::vector<n0::SceneNodePtr>& nodes, bp::Node& node);

private:
	static bp::NodePtr CreateDefault(std::vector<n0::SceneNodePtr>& nodes, bp::Node& parent,
		int idx, const std::string& type);

}; // NodeBuilder

}