#pragma once

#include "shadergraph/Node.h"
#include "shadergraph/Pins.h"
#include "shadergraph/ReflectPropTypes.h"

namespace sg
{
namespace node
{

class Log : public Node
{
public:
	Log()
		: Node("Log", true)
	{
		InitPins({
			{ PINS_DYNAMIC_VECTOR, "In" },
		},{
			{ PINS_DYNAMIC_VECTOR, "Out", }
		});
	}

	PropMathBaseType GetType() const { return m_type; }
	void SetType(PropMathBaseType type) { m_type = type; }

private:
	PropMathBaseType m_type = PropMathBaseType::BASE_E;

	RTTR_ENABLE(Node)

}; // Log

}
}