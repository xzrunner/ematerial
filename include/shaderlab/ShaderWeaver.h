#pragma once

#include <blueprint/typedef.h>
#include <blueprint/node/SetReference.h>

#include <shaderweaver/typedef.h>
#include <shaderweaver/Node.h>
#include <painting0/Shader.h>
#include <painting3/GlobalIllumination.h>

#include <vector>

namespace bp { class Node; }
namespace pt3 { struct GlobalIllumination; }
namespace sw { class Evaluator; }
namespace ur { class ShaderProgram; }

namespace shaderlab
{

// shader graph -> shader weaver -> shader string
class ShaderWeaver
{
public:
	enum ShaderType
	{
		SHADER_SHAPE = 0,
		SHADER_SPRITE,
		SHADER_PHONG,
        SHADER_PBR,
        SHADER_RAYMARCHING,
	};

public:
    ShaderWeaver(const std::vector<bp::NodePtr>& all_nodes);
	ShaderWeaver(ShaderType shader_type, const bp::Node& frag_node,
		bool debug_print = false, const std::vector<bp::NodePtr>& all_nodes = std::vector<bp::NodePtr>(),
        const pt3::GlobalIllumination& gi = pt3::GlobalIllumination());

	std::shared_ptr<ur::ShaderProgram> CreateShader2() const;
	std::shared_ptr<ur::ShaderProgram> CreateShader3() const;

	sw::NodePtr CreateWeaverNode(const bp::Node& node);

private:
    void PrepareSetRefNodes(const std::vector<bp::NodePtr>& all_nodes);

	bool CreateFromNode(const bp::Node& node, int input_idx, sw::Node::PortAddr& from_port);

//	pt0::Shader::Params CreateShaderParams(const sw::Evaluator& vert, const sw::Evaluator& frag) const;

private:
	bool m_debug_print = false;

    std::map<std::string, std::shared_ptr<const bp::node::SetReference>> m_map2setnodes;

	std::vector<sw::NodePtr> m_cached_nodes;

	std::vector<sw::NodePtr> m_vert_nodes;
	sw::NodePtr m_frag_node = nullptr;

//	std::vector<ur::VertexAttrib> m_layout;

	// textures
	std::vector<std::string> m_texture_names;
	std::vector<uint32_t>    m_texture_ids;

}; // ShaderWeaver

}