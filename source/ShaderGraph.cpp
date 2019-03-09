#include "shadergraph/ShaderGraph.h"
#include "shadergraph/NodeBuilder.h"
#include "shadergraph/NodeHelper.h"
#include "shadergraph/Node.h"
#include "shadergraph/PinsCallback.h"
#include "shadergraph/TypeDeduction.h"

#include <blueprint/NodeBuilder.h>
#include <blueprint/Pins.h>
#include <shaderweaver/ShaderWeaver.h>

namespace sg
{

CU_SINGLETON_DEFINITION(ShaderGraph);

extern void regist_rttr();

ShaderGraph::ShaderGraph()
{
	sw::ShaderWeaver::Instance();

	regist_rttr();

	Init();
	InitNodes();

    InitPinsCallback();
}

void ShaderGraph::Init()
{
	bp::NodeBuilder::Callback cb;
	cb.after_created = [](bp::Node& node, std::vector<n0::SceneNodePtr>& nodes) {
		NodeBuilder::CreateDefaultInputs(nodes, node);
	};
	cb.before_connected = [](bp::Pins& from, bp::Pins& to) {
		NodeHelper::RemoveDefaultNode(to);
	};
	cb.after_connected = [](bp::Pins& from, bp::Pins& to) {
        TypeDeduction::DeduceConn(from, to);
	};
	bp::NodeBuilder::Instance()->RegistCB(cb);
}

void ShaderGraph::InitNodes()
{
	auto list = rttr::type::get<Node>().get_derived_classes();
	m_nodes.reserve(list.size());
	for (auto& t : list)
	{
		auto obj = t.create();
		assert(obj.is_valid());
		auto node = obj.get_value<bp::NodePtr>();
		assert(node);
		m_nodes.push_back(node);
	}
}

}