#pragma once

#include "shadergraph/Node.h"
#include "shadergraph/Pins.h"

namespace sg
{
namespace node
{

class Spherize : public Node
{
public:
	Spherize()
		: Node("Spherize", true)
	{
		InitPins({
			{ PINS_VECTOR2, "UV" },
			{ PINS_VECTOR2, "Center" },
			{ PINS_VECTOR2, "Strength" },
			{ PINS_VECTOR2, "Offset" },
		},{
			{ PINS_VECTOR2, "Out", }
		});
	}

public:
	enum InputID
	{
		ID_UV = 0,
		ID_CENTER,
		ID_STRENGTH,
		ID_OFFSET,
	};

	DECLARE_NODE_CLASS(Spherize)

}; // Spherize

}
}