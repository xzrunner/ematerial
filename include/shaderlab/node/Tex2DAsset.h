#pragma once

#include "shaderlab/Node.h"
#include "shaderlab/PinType.h"

namespace facade { class Image; }

namespace shaderlab
{
namespace node
{

class Tex2DAsset : public Node
{
public:
	Tex2DAsset()
		: Node("Tex2DAsset", false)
		, m_name("tex")
	{
		InitPins({
		},{
			{ PIN_TEXTURE2D, "Tex" }
		});
	}

	virtual void Draw(const ur::Device& dev, ur::Context& ctx,
        const n2::RenderParams& rp) const override;

	void  SetName(const std::string& name);
	auto& GetName() const { return m_name; }

	void SetImagePath(std::string filepath);
	std::string GetImagePath() const;

	auto& GetImage() const { return m_img; }

private:
	std::string m_name;

	std::shared_ptr<facade::Image> m_img = nullptr;

	RTTR_ENABLE(Node)

}; // Tex2DAsset

}
}