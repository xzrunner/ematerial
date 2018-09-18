#pragma once

#include "shadergraph/Node.h"
#include "shadergraph/Pins.h"

namespace sg
{
namespace node
{

class TextureSample : public Node
{
public:
	TextureSample()
		: Node("TextureSample", true)
	{
		InitPins({
			{ PINS_TEXTURE2D, "Tex" },
			{ PINS_VECTOR2,   "UV" },
		},{
			{ PINS_VECTOR4, "RGBA" },
			{ PINS_VECTOR1, "R" },
			{ PINS_VECTOR1, "G" },
			{ PINS_VECTOR1, "B" },
			{ PINS_VECTOR1, "A" },
		});
	}

public:
	enum InputId
	{
		ID_TEX = 0,
		ID_UV,
	};

	enum OutputId
	{
		ID_RGBA = 0,
		ID_R,
		ID_G,
		ID_B,
		ID_A,
	};

	DECLARE_NODE_CLASS(TextureSample)

}; // TextureSample

}
}