#include "shadergraph/Subtract.h"
#include "shadergraph/Utility.h"
#include "shadergraph/PinsType.h"

namespace shadergraph
{

const std::string Subtract::TYPE_NAME = "mat_sub";

Subtract::Subtract()
	: Node("Subtract")
{
	AddPins(m_input0 = std::make_shared<bp::node::Pins>(
		true, 0, PINS_VECTOR1, "", *this, true));
	AddPins(m_input1 = std::make_shared<bp::node::Pins>(
		true, 1, PINS_VECTOR1, "", *this, true));

	AddPins(m_output = std::make_shared<bp::node::Pins>(
		false, 0, PINS_VECTOR1, "", *this, true));

	Layout();
}

sm::vec3 Subtract::ToVec3() const
{
	sm::vec3 sub0, sub1;
	if (m_input0) {
		sub0 = Utility::CalcNodeInputVal(*m_input0);
	}
	if (m_input1) {
		sub1 = Utility::CalcNodeInputVal(*m_input1);
	}
	return sub0 - sub1;
}

}