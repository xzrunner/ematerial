#pragma once

#include <sw/typedef.h>
#include <unirender/VertexAttrib.h>

#include <vector>

namespace bp { class Node; }
namespace pt2 { class WindowContext; class Shader; }
namespace pt3 { class WindowContext; class Shader; }

namespace sg
{

// shader graph -> shader weaver -> shader string
class ShaderWeaver
{
public:
	enum VertType
	{
		VERT_SHAPE = 0,
		VERT_SPRITE,
		VERT_PHONG
	};

public:
	ShaderWeaver(VertType vert_type, const bp::Node& frag_node,
		bool debug_print = false);

	std::shared_ptr<pt2::Shader> CreateShader(pt2::WindowContext& wc) const;
	std::shared_ptr<pt3::Shader> CreateShader(pt3::WindowContext& wc) const;

private:
	sw::NodePtr CreateWeaverNode(const bp::Node& node);
	sw::NodePtr CreateInputChild(const bp::Node& node, int input_idx);

private:
	bool m_debug_print;

	std::vector<sw::NodePtr> m_cached_nodes;

	std::vector<sw::NodePtr> m_vert_nodes;
	sw::NodePtr m_frag_node = nullptr;

	std::vector<ur::VertexAttrib> m_layout;

	// textures
	std::vector<std::string> m_texture_names;
	std::vector<uint32_t>    m_texture_ids;

}; // ShaderWeaver

}