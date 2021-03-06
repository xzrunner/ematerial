#include "shaderlab/ShaderWeaver.h"
#include "shaderlab/RegistNodes.h"
#include "shaderlab/node/Custom.h"
#include "shaderlab/node/Tex2DAsset.h"
#include "shaderlab/node/TexCubeAsset.h"

#include <shaderweaver/node/ShaderUniform.h>
#include <shaderweaver/node/ShaderInput.h>
#include <shaderweaver/node/ShaderOutput.h>
#include <shaderweaver/node/PositionTrans.h>
#include <shaderweaver/node/FragPosTrans.h>
#include <shaderweaver/node/NormalTrans.h>
#include <shaderweaver/node/PBR.h>
#include <shaderweaver/node/IBL.h>
#include <shaderweaver/node/Phong.h>
#include <shaderweaver/node/Time.h>
#include <shaderweaver/node/Hue.h>
#include <shaderweaver/node/InvertColors.h>
#include <shaderweaver/node/Blend.h>
#include <shaderweaver/node/ChannelMask.h>
#include <shaderweaver/node/ColorspaceConversion.h>
#include <shaderweaver/node/RGBToHSV.h>
#include <shaderweaver/node/HSVToRGB.h>
#include <shaderweaver/node/Flip.h>
#include <shaderweaver/node/Swizzle.h>
#include <shaderweaver/node/Boolean.h>
#include <shaderweaver/node/Vector1.h>
#include <shaderweaver/node/Vector2.h>
#include <shaderweaver/node/Vector3.h>
#include <shaderweaver/node/Vector4.h>
#include <shaderweaver/node/UV.h>
#include <shaderweaver/node/ViewDirection.h>
#include <shaderweaver/node/WorldSpaceLightDir.h>
#include <shaderweaver/node/CameraPos.h>
#include <shaderweaver/node/Matrix2.h>
#include <shaderweaver/node/Matrix3.h>
#include <shaderweaver/node/Matrix4.h>
#include <shaderweaver/node/Exponential.h>
#include <shaderweaver/node/Log.h>
#include <shaderweaver/node/MatrixConstruction.h>
#include <shaderweaver/node/MatrixSplit.h>
#include <shaderweaver/node/Comparison.h>
#include <shaderweaver/node/Flipbook.h>
#include <shaderweaver/node/Rotate.h>
#include <shaderweaver/node/Raymarching.h>
#include <shaderweaver/node/Custom.h>
#include <shaderweaver/node/Function.h>
#include <shaderweaver/node/FuncInput.h>
#include <shaderweaver/node/FuncOutput.h>
#include <shaderweaver/node/VertexShader.h>
#include <shaderweaver/node/FragmentShader.h>
#include <shaderweaver/node/Add.h>
#include <shaderweaver/node/Multiply.h>
#include <shaderweaver/node/Tonemap.h>
#include <shaderweaver/node/GammaCorrect.h>
#include <shaderweaver/node/SampleCube.h>

#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>
#include <blueprint/CompNode.h>
#include <blueprint/node/GetReference.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>
#include <shaderweaver/Evaluator.h>
#include <unirender/Texture.h>
#include <painting2/Shader.h>
#include <painting3/Shader.h>
#include <node0/SceneNode.h>
#include <facade/Image.h>
#include <facade/ImageCube.h>

#include <assert.h>

namespace
{

const char* PROJ_MAT_NAME  = "u_projection";
const char* VIEW_MAT_NAME  = "u_view";
const char* MODEL_MAT_NAME = "u_model";

const char* VERT_POSITION_NAME  = "position";
const char* VERT_NORMAL_NAME    = "normal";
const char* VERT_TEXCOORD_NAME  = "texcoord";

const char* FRAG_POSITION_NAME = "v_world_pos";
const char* FRAG_NORMAL_NAME   = "v_normal";
const char* FRAG_TEXCOORD_NAME = "v_texcoord";

void debug_print(const sw::Evaluator& vert, const sw::Evaluator& frag)
{
	printf("//////////////////////////////////////////////////////////////////////////\n");
	printf("%s\n", vert.GenShaderStr().c_str());
	printf("//////////////////////////////////////////////////////////////////////////\n");
	printf("%s\n", frag.GenShaderStr().c_str());
	printf("//////////////////////////////////////////////////////////////////////////\n");
}

uint32_t var_type_sg_to_sw(int type)
{
    uint32_t ret = 0;
    switch (type)
    {
    case shaderlab::PIN_BOOLEAN:
        ret = sw::t_bool;
        break;
    case shaderlab::PIN_DYNAMIC_VECTOR:
        ret = sw::t_d_vec;
        break;
    case shaderlab::PIN_VECTOR1:
        ret = sw::t_flt1;
        break;
    case shaderlab::PIN_VECTOR2:
        ret = sw::t_flt2;
        break;
    case shaderlab::PIN_VECTOR3:
        ret = sw::t_flt3;
        break;
    case shaderlab::PIN_VECTOR4:
        ret = sw::t_flt4;
        break;
    case shaderlab::PIN_COLOR:
        ret = sw::t_col3;
        break;
    case shaderlab::PIN_TEXTURE2D:
        ret = sw::t_tex2d;
        break;
    case shaderlab::PIN_CUBE_MAP:
        break;
    case shaderlab::PIN_DYNAMIC_MATRIX:
        ret = sw::t_d_mat;
        break;
    case shaderlab::PIN_MATRIX2:
        ret = sw::t_mat2;
        break;
    case shaderlab::PIN_MATRIX3:
        ret = sw::t_mat3;
        break;
    case shaderlab::PIN_MATRIX4:
        ret = sw::t_mat4;
        break;
    case shaderlab::PIN_FUNCTION:
        ret = sw::t_func;
        break;
    }
    return ret;
}

/*
    v_texcoord = a_tex_coords;
    v_world_pos = vec3(u_model * a_pos);
    v_normal = mat3(u_model) * a_normal;

    gl_Position =  u_projection * u_view * u_model * a_pos;
*/
void init_vert3d(std::vector<sw::NodePtr>& m_cached_nodes, std::vector<sw::NodePtr>& m_vert_nodes)
{
	auto projection = std::make_shared<sw::node::ShaderUniform>(PROJ_MAT_NAME,  sw::t_mat4);
	auto view       = std::make_shared<sw::node::ShaderUniform>(VIEW_MAT_NAME,  sw::t_mat4);
	auto model      = std::make_shared<sw::node::ShaderUniform>(MODEL_MAT_NAME, sw::t_mat4);
	m_cached_nodes.push_back(projection);
	m_cached_nodes.push_back(view);
	m_cached_nodes.push_back(model);

	auto position = std::make_shared<sw::node::ShaderInput>(VERT_POSITION_NAME, sw::t_flt3);
	auto normal   = std::make_shared<sw::node::ShaderInput>(VERT_NORMAL_NAME,   sw::t_nor3);
    auto texcoord = std::make_shared<sw::node::ShaderInput>(VERT_TEXCOORD_NAME, sw::t_uv);
	m_cached_nodes.push_back(position);
	m_cached_nodes.push_back(normal);
    m_cached_nodes.push_back(texcoord);

    // gl_Position =  u_projection * u_view * u_model * a_pos;
	auto pos_trans = std::make_shared<sw::node::PositionTrans>(4);
	sw::make_connecting({ projection, 0 }, { pos_trans, sw::node::PositionTrans::ID_PROJ });
	sw::make_connecting({ view, 0 },       { pos_trans, sw::node::PositionTrans::ID_VIEW });
	sw::make_connecting({ model, 0 },      { pos_trans, sw::node::PositionTrans::ID_MODEL });
	sw::make_connecting({ position, 0 },   { pos_trans, sw::node::PositionTrans::ID_POS });
    m_cached_nodes.push_back(pos_trans);
    auto vert_end = std::make_shared<sw::node::VertexShader>();
    sw::make_connecting({ pos_trans, 0 }, { vert_end, 0 });
    m_vert_nodes.push_back(vert_end);

	auto frag_pos_trans = std::make_shared<sw::node::FragPosTrans>();
	sw::make_connecting({ model, 0 },    { frag_pos_trans, sw::node::FragPosTrans::ID_MODEL });
	sw::make_connecting({ position, 0 }, { frag_pos_trans, sw::node::FragPosTrans::ID_POS });
    m_cached_nodes.push_back(frag_pos_trans);

    auto normal_mat = std::make_shared<sw::node::ShaderUniform>(sw::node::NormalTrans::NormalMatName(), sw::t_mat3);
    m_cached_nodes.push_back(normal_mat);
	auto norm_trans = std::make_shared<sw::node::NormalTrans>();
	sw::make_connecting({ normal_mat, 0 }, { norm_trans, sw::node::NormalTrans::ID_NORMAL_MAT });
	sw::make_connecting({ normal, 0 },     { norm_trans, sw::node::NormalTrans::ID_NORMAL });
    m_cached_nodes.push_back(norm_trans);

    // v_texcoord = a_texcoord;
    auto v_texcoord = std::make_shared<sw::node::ShaderOutput>(FRAG_TEXCOORD_NAME, sw::t_uv);
    sw::make_connecting({ texcoord, 0 }, { v_texcoord, 0 });
    m_vert_nodes.push_back(v_texcoord);

    // v_world_pos = vec3(u_model * a_pos);
    auto v_world_pos = std::make_shared<sw::node::ShaderOutput>(FRAG_POSITION_NAME, sw::t_flt3);
    sw::make_connecting({ frag_pos_trans, 0 }, { v_world_pos, 0 });
    m_vert_nodes.push_back(v_world_pos);

    // v_normal = mat3(transpose(inverse(u_model))) * a_normal;
    auto v_normal = std::make_shared<sw::node::ShaderOutput>(FRAG_NORMAL_NAME, sw::t_nor3);
    sw::make_connecting({ norm_trans, 0 }, { v_normal, 0 });
    m_vert_nodes.push_back(v_normal);
}

}

namespace shaderlab
{

ShaderWeaver::ShaderWeaver(const std::vector<bp::NodePtr>& all_nodes)
{
    PrepareSetRefNodes(all_nodes);
}

ShaderWeaver::ShaderWeaver(ShaderType shader_type, const bp::Node& frag_node,
                           bool debug_print, const std::vector<bp::NodePtr>& all_nodes,
                           const pt3::GlobalIllumination& gi)
	: m_debug_print(debug_print)
{
 //   sw::NodePtr frag_end = nullptr;

 //   PrepareSetRefNodes(all_nodes);

 //   switch (shader_type)
	//{
	//case SHADER_SHAPE:
	//{
	//	// layout
	//	m_layout.push_back(ur::VertexAttrib(VERT_POSITION_NAME, 2, 4, 8, 0));

	//	// vert

	//	auto proj  = std::make_shared<sw::node::ShaderUniform>(PROJ_MAT_NAME,  sw::t_mat4);
	//	auto view  = std::make_shared<sw::node::ShaderUniform>(VIEW_MAT_NAME,  sw::t_mat4);
	//	auto model = std::make_shared<sw::node::ShaderUniform>(MODEL_MAT_NAME, sw::t_mat4);
	//	m_cached_nodes.push_back(proj);
	//	m_cached_nodes.push_back(view);
	//	m_cached_nodes.push_back(model);

	//	auto position = std::make_shared<sw::node::ShaderInput>(VERT_POSITION_NAME, sw::t_pos2);
	//	m_cached_nodes.push_back(position);

	//	auto pos_trans = std::make_shared<sw::node::PositionTrans>(2);
	//	sw::make_connecting({ proj, 0 },     { pos_trans, sw::node::PositionTrans::ID_PROJ });
	//	sw::make_connecting({ view, 0 },     { pos_trans, sw::node::PositionTrans::ID_VIEW });
	//	sw::make_connecting({ model, 0 },    { pos_trans, sw::node::PositionTrans::ID_MODEL });
	//	sw::make_connecting({ position, 0 }, { pos_trans, sw::node::PositionTrans::ID_POS });
 //       m_cached_nodes.push_back(pos_trans);
 //       auto vert_end = std::make_shared<sw::node::VertexShader>();
 //       sw::make_connecting({ pos_trans, 0 }, { vert_end, 0 });
	//	m_vert_nodes.push_back(vert_end);

 //       frag_end = CreateWeaverNode(frag_node);
	//}
	//	break;
	//case SHADER_SPRITE:
	//{
	//	// layout
	//	m_layout.push_back(ur::VertexAttrib(VERT_POSITION_NAME, 2, 4, 16, 0));
	//	m_layout.push_back(ur::VertexAttrib(VERT_TEXCOORD_NAME, 2, 4, 16, 8));

	//	// vert

	//	auto proj  = std::make_shared<sw::node::ShaderUniform>(PROJ_MAT_NAME,  sw::t_mat4);
	//	auto view  = std::make_shared<sw::node::ShaderUniform>(VIEW_MAT_NAME,  sw::t_mat4);
	//	auto model = std::make_shared<sw::node::ShaderUniform>(MODEL_MAT_NAME, sw::t_mat4);
	//	m_cached_nodes.push_back(proj);
	//	m_cached_nodes.push_back(view);
	//	m_cached_nodes.push_back(model);

	//	auto position = std::make_shared<sw::node::ShaderInput>(VERT_POSITION_NAME, sw::t_pos2);
	//	m_cached_nodes.push_back(position);

 //       // gl_Position =  u_projection * u_view * u_model * a_pos;
	//	auto pos_trans = std::make_shared<sw::node::PositionTrans>(2);
	//	sw::make_connecting({ proj, 0 },     { pos_trans, sw::node::PositionTrans::ID_PROJ });
	//	sw::make_connecting({ view, 0 },     { pos_trans, sw::node::PositionTrans::ID_VIEW });
	//	sw::make_connecting({ model, 0 },    { pos_trans, sw::node::PositionTrans::ID_MODEL });
	//	sw::make_connecting({ position, 0 }, { pos_trans, sw::node::PositionTrans::ID_POS });
 //       m_cached_nodes.push_back(pos_trans);
 //       auto vert_end = std::make_shared<sw::node::VertexShader>();
 //       sw::make_connecting({ pos_trans, 0 }, { vert_end, 0 });
 //       m_vert_nodes.push_back(vert_end);

 //       // v_texcoord = a_texcoord;
 //       auto a_texcoord = std::make_shared<sw::node::ShaderInput>(VERT_TEXCOORD_NAME, sw::t_uv);
 //       auto v_texcoord = std::make_shared<sw::node::ShaderOutput>(FRAG_TEXCOORD_NAME, sw::t_uv);
 //       sw::make_connecting({ a_texcoord, 0 }, { v_texcoord, 0 });
 //       m_vert_nodes.push_back(v_texcoord);
 //       m_cached_nodes.push_back(a_texcoord);

	//	// frag
	//	frag_end = CreateWeaverNode(frag_node);

 //       // fixme
 //       if (frag_end && frag_end->get_type() == rttr::type::get<sw::node::Raymarching>()) {
 //           auto cam_pos = std::make_shared<sw::node::CameraPos>();
 //           m_cached_nodes.push_back(cam_pos);
 //           sw::make_connecting({ cam_pos, 0 }, { frag_end, sw::node::Raymarching::ID_CAM_POS });
 //       }
	//}
	//	break;
	//case SHADER_PHONG:
	//{
	//	// layout
	//	m_layout.push_back(ur::VertexAttrib(VERT_POSITION_NAME, 3, 4, 32, 0));
	//	m_layout.push_back(ur::VertexAttrib(VERT_NORMAL_NAME,   3, 4, 32, 12));
	//	m_layout.push_back(ur::VertexAttrib(VERT_TEXCOORD_NAME, 2, 4, 32, 24));

	//	// vert
 //       init_vert3d(m_cached_nodes, m_vert_nodes);

	//	// frag
	//	auto phong = CreateWeaverNode(frag_node);

 //       auto cam_pos = std::make_shared<sw::node::CameraPos>();
	//	auto frag_in_pos = std::make_shared<sw::node::ShaderInput>(FRAG_POSITION_NAME, sw::t_flt3);
	//	auto frag_in_nor = std::make_shared<sw::node::ShaderInput>(FRAG_NORMAL_NAME, sw::t_nor3);
 //       m_cached_nodes.push_back(cam_pos);
	//	m_cached_nodes.push_back(frag_in_pos);
	//	m_cached_nodes.push_back(frag_in_nor);
 //       sw::make_connecting({ cam_pos, 0 }, { phong, sw::node::Phong::ID_VIEW_POS });
	//	sw::make_connecting({ frag_in_pos, 0 }, { phong, sw::node::Phong::ID_FRAG_POS });
	//	sw::make_connecting({ frag_in_nor, 0 }, { phong, sw::node::Phong::ID_NORMAL });

	//	frag_end = phong;
	//}
	//	break;
 //   case SHADER_PBR:
 //   {
	//	// layout
	//	m_layout.push_back(ur::VertexAttrib(VERT_POSITION_NAME, 3, 4, 32, 0));
	//	m_layout.push_back(ur::VertexAttrib(VERT_NORMAL_NAME,   3, 4, 32, 12));
	//	m_layout.push_back(ur::VertexAttrib(VERT_TEXCOORD_NAME, 2, 4, 32, 24));

 //       // vert
 //       init_vert3d(m_cached_nodes, m_vert_nodes);

 //       // PBR
 //       auto pbr = CreateWeaverNode(frag_node);

 //       auto frag_in_pos = std::make_shared<sw::node::ShaderInput>(FRAG_POSITION_NAME, sw::t_flt3);
 //       auto frag_in_nor = std::make_shared<sw::node::ShaderInput>(FRAG_NORMAL_NAME,    sw::t_nor3);
 //       auto frag_in_tex = std::make_shared<sw::node::ShaderInput>(FRAG_TEXCOORD_NAME,  sw::t_uv);
 //       auto cam_pos = std::make_shared<sw::node::CameraPos>();
 //       m_cached_nodes.push_back(frag_in_pos);
 //       m_cached_nodes.push_back(frag_in_nor);
 //       m_cached_nodes.push_back(frag_in_tex);
 //       m_cached_nodes.push_back(cam_pos);
 //       sw::make_connecting({ frag_in_pos, 0 }, { pbr, sw::node::PBR::ID_FRAG_POS });
 //       sw::make_connecting({ frag_in_nor, 0 }, { pbr, sw::node::PBR::ID_NORMAL });
 //       sw::make_connecting({ frag_in_tex, 0 }, { pbr, sw::node::PBR::ID_TEXCOORD });
 //       sw::make_connecting({ cam_pos, 0 },     { pbr, sw::node::PBR::ID_CAM_POS });

 //       sw::Node::PortAddr ambient_out;
 //       // IBL
 //       if (gi.irradiance_map)
 //       {
 //           auto ibl = std::make_shared<sw::node::IBL>();
 //           m_cached_nodes.push_back(ibl);
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_N },         { ibl, sw::node::IBL::ID_N });
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_V },         { ibl, sw::node::IBL::ID_V });
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_F0 },        { ibl, sw::node::IBL::ID_F0 });
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_ALBEDO },    { ibl, sw::node::IBL::ID_ALBEDO });
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_METALLIC },  { ibl, sw::node::IBL::ID_METALLIC });
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_ROUGHNESS }, { ibl, sw::node::IBL::ID_ROUGHNESS });
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_AO },        { ibl, sw::node::IBL::ID_AO });

 //           auto irr_map_name = sw::node::IBL::IrradianceMapName();
 //           m_texture_names.push_back(irr_map_name);
 //           m_texture_ids.push_back(gi.irradiance_map->GetTexID());
 //           auto irradiance_map = std::make_shared<sw::node::ShaderUniform>();
 //           irradiance_map->SetNameAndType(irr_map_name, sw::t_tex_cube);
 //           m_cached_nodes.push_back(irradiance_map);
 //           sw::make_connecting({ irradiance_map, 0 }, { ibl, sw::node::IBL::ID_IRRADIANCE_MAP });

 //           auto pre_map_name = sw::node::IBL::PrefilterMapName();
 //           m_texture_names.push_back(pre_map_name);
 //           m_texture_ids.push_back(gi.prefilter_map->GetTexID());
 //           auto prefilter_map = std::make_shared<sw::node::ShaderUniform>();
 //           prefilter_map->SetNameAndType(pre_map_name, sw::t_tex_cube);
 //           m_cached_nodes.push_back(prefilter_map);
 //           sw::make_connecting({ prefilter_map, 0 }, { ibl, sw::node::IBL::ID_PREFILTER_MAP });

 //           auto brdf_lut_name = sw::node::IBL::BrdfLutName();
 //           m_texture_names.push_back(brdf_lut_name);
 //           m_texture_ids.push_back(gi.brdf_lut->TexID());
 //           auto brdf_lut = std::make_shared<sw::node::ShaderUniform>();
 //           brdf_lut->SetNameAndType(brdf_lut_name, sw::t_tex2d);
 //           m_cached_nodes.push_back(brdf_lut);
 //           sw::make_connecting({ brdf_lut, 0 }, { ibl, sw::node::IBL::ID_BRDF_LUT });

 //           ambient_out = { ibl, 0 };
 //       }
 //       else
 //       {
 //           // vec3 ambient = vec3(0.03) * albedo * ao;
 //           auto ambient = std::make_shared<sw::node::Multiply>();
 //           m_cached_nodes.push_back(ambient);
 //           ambient->SetInputPortCount(3);
 //           auto f = std::make_shared<sw::node::Vector1>("", 0.03f);
 //           m_cached_nodes.push_back(f);
 //           sw::make_connecting({ f, 0 },                          { ambient, 0 });
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_ALBEDO }, { ambient, 1 });
 //           sw::make_connecting({ pbr, sw::node::PBR::ID_AO },     { ambient, 2 });

 //           ambient_out = { ambient, 0 };
 //       }

 //       // vec3 color = ambient + Lo;
 //       auto color = std::make_shared<sw::node::Add>();
 //       m_cached_nodes.push_back(color);
 //       sw::make_connecting(ambient_out,                   { color, 0 });
 //       sw::make_connecting({ pbr, sw::node::PBR::ID_Lo }, { color, 1 });

 //       // HDR tonemapping
 //       auto tonemap = std::make_shared<sw::node::Tonemap>();
 //       m_cached_nodes.push_back(tonemap);
 //       sw::make_connecting({ color, 0 }, { tonemap, 0 });

 //       // gamma correct
 //       auto gamma = std::make_shared<sw::node::GammaCorrect>();
 //       m_cached_nodes.push_back(gamma);
 //       sw::make_connecting({ tonemap, 0 }, { gamma, 0 });

 //       frag_end = gamma;
 //   }
 //       break;
	//case SHADER_RAYMARCHING:
	//{
	//	// layout
	//	m_layout.push_back(ur::VertexAttrib(VERT_POSITION_NAME, 3, 4, 32, 0));

	//	// vert
	//	auto projection = std::make_shared<sw::node::ShaderUniform>(PROJ_MAT_NAME,  sw::t_mat4);
	//	auto view       = std::make_shared<sw::node::ShaderUniform>(VIEW_MAT_NAME,  sw::t_mat4);
	//	auto model      = std::make_shared<sw::node::ShaderUniform>(MODEL_MAT_NAME, sw::t_mat4);
	//	m_cached_nodes.push_back(projection);
	//	m_cached_nodes.push_back(view);
	//	m_cached_nodes.push_back(model);

	//	auto position = std::make_shared<sw::node::ShaderInput>(VERT_POSITION_NAME, sw::t_flt3);
	//	m_cached_nodes.push_back(position);

	//	auto pos_trans = std::make_shared<sw::node::PositionTrans>(4);
	//	sw::make_connecting({ projection, 0 }, { pos_trans, sw::node::PositionTrans::ID_PROJ });
	//	sw::make_connecting({ view, 0 },       { pos_trans, sw::node::PositionTrans::ID_VIEW });
	//	sw::make_connecting({ model, 0 },      { pos_trans, sw::node::PositionTrans::ID_MODEL });
	//	sw::make_connecting({ position, 0 },   { pos_trans, sw::node::PositionTrans::ID_POS });
 //       m_cached_nodes.push_back(pos_trans);
 //       auto vert_end = std::make_shared<sw::node::VertexShader>();
 //       sw::make_connecting({ pos_trans, 0 }, { vert_end, 0 });
	//	m_vert_nodes.push_back(vert_end);

	//	// frag
	//	auto raymarching = CreateWeaverNode(frag_node);

 //       auto cam_pos = std::make_shared<sw::node::CameraPos>();
 //       m_cached_nodes.push_back(cam_pos);
 //       sw::make_connecting({ cam_pos, 0 }, { raymarching, sw::node::Raymarching::ID_CAM_POS });

	//	frag_end = raymarching;
	//}
	//	break;
	//default:
	//	assert(0);
	//}

 //   if (frag_end) {
 //       m_frag_node = std::make_shared<sw::node::FragmentShader>();
 //       sw::make_connecting({ frag_end, 0 }, { m_frag_node, 0 });
 //   }
}

std::shared_ptr<ur::ShaderProgram> ShaderWeaver::CreateShader2() const
{
	//sw::Evaluator vert(m_vert_nodes);
	//sw::Evaluator frag({ m_frag_node });

	//if (m_debug_print) {
	//	debug_print(vert, frag);
	//}

	//auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	//auto shader = std::make_shared<pt2::Shader>(&rc, CreateShaderParams(vert, frag));

	//shader->SetUsedTextures(m_texture_ids);

	//return shader;

    return nullptr;
}

std::shared_ptr<ur::ShaderProgram> ShaderWeaver::CreateShader3() const
{
	//sw::Evaluator vert(m_vert_nodes);
	//sw::Evaluator frag({ m_frag_node });

	//if (m_debug_print) {
	//	debug_print(vert, frag);
	//}

	//auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	//auto shader = std::make_shared<pt3::Shader>(&rc, CreateShaderParams(vert, frag));

	//shader->SetUsedTextures(m_texture_ids);

	//return shader;

    return nullptr;
}

sw::NodePtr ShaderWeaver::CreateWeaverNode(const bp::Node& node)
{
	// create node
	sw::NodePtr dst = nullptr;
	auto type = node.get_type();
    if (type == rttr::type::get<bp::node::SetReference>())
    {
        auto& set_ref = static_cast<const bp::node::SetReference&>(node);
        auto& conns = set_ref.GetAllInput()[0]->GetConnecting();
        if (conns.empty()) {
            return nullptr;
        } else {
            assert(conns.size() == 1 && conns[0]->GetFrom());
            return CreateWeaverNode(conns[0]->GetFrom()->GetParent());
        }
    }
    else if (type == rttr::type::get<bp::node::GetReference>())
    {
        auto& get_node = static_cast<const bp::node::GetReference&>(node);
        auto itr = m_map2setnodes.find(get_node.GetName());
        if (itr != m_map2setnodes.end())
        {
            auto& conns = itr->second->GetAllInput()[0]->GetConnecting();
            if (conns.empty()) {
                return nullptr;
            } else {
                assert(conns.size() == 1 && conns[0]->GetFrom());
                return CreateWeaverNode(conns[0]->GetFrom()->GetParent());
            }
        }
    }
    else if (type == rttr::type::get<bp::node::Function>())
    {
        dst = std::make_shared<sw::node::Function>();
    }
	else if (type == rttr::type::get<shaderlab::node::Tex2DAsset>())
	{
        auto& tex2d = static_cast<const shaderlab::node::Tex2DAsset&>(node);
		dst = std::make_shared<sw::node::ShaderUniform>(tex2d.GetName(), sw::t_tex2d);
	}
    else if (type == rttr::type::get<shaderlab::node::TexCubeAsset>())
    {
        auto& tex_cube = static_cast<const shaderlab::node::TexCubeAsset&>(node);
        dst = std::make_shared<sw::node::ShaderUniform>(tex_cube.GetName(), sw::t_tex_cube);
    }
	else
	{
        auto src_type = type.get_name().to_string();
        std::string dst_type;
        auto find_sg = src_type.find("shaderlab::");
        if (find_sg != std::string::npos)
        {
            dst_type = "sw::" + src_type.substr(find_sg + strlen("shaderlab::"));
        }
        else
        {
            if (src_type == "bp::Input") {
                dst_type = "sw::FuncInput";
            } else if (src_type == "bp::Output") {
                dst_type = "sw::FuncOutput";
            }
        }
        if (dst_type.empty()) {
            return nullptr;
        }

		rttr::type t = rttr::type::get_by_name(dst_type);
		if (!t.is_valid()) {
			return nullptr;
		}
		assert(t.is_valid());
		rttr::variant var = t.create();
		assert(var.is_valid());

		dst = var.get_value<std::shared_ptr<sw::Node>>();
		assert(dst);
	}

    if (!dst) {
        return nullptr;
    }

	// init
    if (type == rttr::type::get<node::Add>())
    {
        auto& src = static_cast<const node::Add&>(node);
        std::static_pointer_cast<sw::node::Add>(dst)->
            SetInputPortCount(src.GetAllInput().size());
    }
    else if (type == rttr::type::get<node::Multiply>())
    {
        auto& src = static_cast<const node::Multiply&>(node);
        std::static_pointer_cast<sw::node::Multiply>(dst)->
            SetInputPortCount(src.GetAllInput().size());
    }
	else if (type == rttr::type::get<node::Hue>())
	{
		auto& src = static_cast<const node::Hue&>(node);
		std::static_pointer_cast<sw::node::Hue>(dst)->
			SetRadians(src.GetAngleType() == PropAngleType::RADIAN);
	}
	else if (type == rttr::type::get<node::InvertColors>())
	{
		auto& src = static_cast<const node::InvertColors&>(node);
		std::static_pointer_cast<sw::node::InvertColors>(dst)->
			SetChannels(src.GetChannels().channels);
	}
	else if (type == rttr::type::get<node::Blend>())
	{
		auto& src = static_cast<const node::Blend&>(node);
		std::static_pointer_cast<sw::node::Blend>(dst)->
			SetModeType(static_cast<sw::node::Blend::ModeType>(src.GetMode()));
	}
	else if (type == rttr::type::get<node::ChannelMask>())
	{
		auto& src = static_cast<const node::ChannelMask&>(node);
		std::static_pointer_cast<sw::node::ChannelMask>(dst)->
			SetChannels(src.GetChannels().channels);
	}
	else if (type == rttr::type::get<node::ColorspaceConversion>())
	{
		auto& src = static_cast<const node::ColorspaceConversion&>(node);
		auto from = static_cast<sw::node::ColorspaceConversion::ColorType>(src.GetType().from);
		auto to = static_cast<sw::node::ColorspaceConversion::ColorType>(src.GetType().to);
		std::static_pointer_cast<sw::node::ColorspaceConversion>(dst)->SetColType(from, to);
	}
	else if (type == rttr::type::get<node::Flip>())
	{
		auto& src = static_cast<const node::Flip&>(node);
		std::static_pointer_cast<sw::node::Flip>(dst)->SetChannels(src.GetChannels().channels);
	}
	else if (type == rttr::type::get<node::Swizzle>())
	{
		auto& src = static_cast<const node::Swizzle&>(node);
		auto& src_channels = src.GetChannels().channels;
		std::array<sw::node::Swizzle::ChannelType, sw::node::Swizzle::CHANNEL_COUNT> dst_channels;
		for (size_t i = 0; i < sw::node::Swizzle::CHANNEL_COUNT; ++i) {
			dst_channels[i] = static_cast<sw::node::Swizzle::ChannelType>(src_channels[i]);
		}
		std::static_pointer_cast<sw::node::Swizzle>(dst)->SetChannels(dst_channels);
	}
	else if (type == rttr::type::get<node::Boolean>())
	{
		auto& src = static_cast<const node::Boolean&>(node);
		auto boolean = std::static_pointer_cast<sw::node::Boolean>(dst);
		boolean->SetName(src.GetName());
		boolean->SetValue(src.GetValue());
	}
	else if (type == rttr::type::get<node::Vector1>())
	{
		auto& src = static_cast<const node::Vector1&>(node);
		auto vec1 = std::static_pointer_cast<sw::node::Vector1>(dst);
		vec1->SetName(src.GetName());
		vec1->SetValue(src.GetValue());
	}
	else if (type == rttr::type::get<node::Vector2>())
	{
		auto& src = static_cast<const node::Vector2&>(node);
		auto vec2 = std::static_pointer_cast<sw::node::Vector2>(dst);
		vec2->SetName(src.GetName());
		vec2->SetValue(src.GetValue());
	}
	else if (type == rttr::type::get<node::Vector3>())
	{
		auto& src = static_cast<const node::Vector3&>(node);
		auto vec3 = std::static_pointer_cast<sw::node::Vector3>(dst);
		vec3->SetName(src.GetName());
		vec3->SetValue(src.GetValue());
	}
	else if (type == rttr::type::get<node::Vector4>())
	{
		auto& src = static_cast<const node::Vector4&>(node);
		auto vec4 = std::static_pointer_cast<sw::node::Vector4>(dst);
		vec4->SetName(src.GetName());
		vec4->SetValue(src.GetValue());
	}
	else if (type == rttr::type::get<node::UV>())
	{
		auto& src = static_cast<const node::UV&>(node);
		std::static_pointer_cast<sw::node::UV>(dst)->SetName(src.GetName());
	}
    else if (type == rttr::type::get<node::ViewDirection>())
    {
        auto& src = static_cast<const node::ViewDirection&>(node);
        auto view_dir = std::static_pointer_cast<sw::node::ViewDirection>(dst);
        view_dir->SetViewSpace(static_cast<sw::node::ViewDirection::ViewSpace>(src.GetViewSpace()));
        view_dir->SetSafeNormalize(src.GetSafeNormalize());
    }
	else if (type == rttr::type::get<node::Matrix2>())
	{
		auto& src = static_cast<const node::Matrix2&>(node);
		auto mat2 = std::static_pointer_cast<sw::node::Matrix2>(dst);
		mat2->SetName(src.GetName());
		mat2->SetValue(src.GetValue());
	}
	else if (type == rttr::type::get<node::Matrix3>())
	{
		auto& src = static_cast<const node::Matrix3&>(node);
		auto mat3 = std::static_pointer_cast<sw::node::Matrix3>(dst);
		mat3->SetName(src.GetName());
		mat3->SetValue(src.GetValue());
	}
	else if (type == rttr::type::get<node::Matrix4>())
	{
		auto& src = static_cast<const node::Matrix4&>(node);
		auto mat4 = std::static_pointer_cast<sw::node::Matrix4>(dst);
		mat4->SetName(src.GetName());
		mat4->SetValue(src.GetValue());
	}
	else if (type == rttr::type::get<node::Tex2DAsset>())
	{
		auto& src = static_cast<const node::Tex2DAsset&>(node);
		m_texture_names.push_back(src.GetName());
		auto& img = src.GetImage();
		if (img) {
			m_texture_ids.push_back(img->GetTexID());
		}
		std::static_pointer_cast<sw::node::ShaderUniform>(dst)->
			SetNameAndType(src.GetName(), sw::t_tex2d);
	}
	else if (type == rttr::type::get<node::TexCubeAsset>())
	{
		auto& src = static_cast<const node::TexCubeAsset&>(node);
		m_texture_names.push_back(src.GetName());
		auto& img = src.GetImage();
		if (img) {
			m_texture_ids.push_back(img->GetTexture()->GetTexID());
		}
		std::static_pointer_cast<sw::node::ShaderUniform>(dst)->
			SetNameAndType(src.GetName(), sw::t_tex2d);
	}
	else if (type == rttr::type::get<node::Exponential>())
	{
		auto& src = static_cast<const node::Exponential&>(node);
		std::static_pointer_cast<sw::node::Exponential>(dst)->
			SetType(static_cast<sw::node::Exponential::BaseType>(src.GetType()));
	}
	else if (type == rttr::type::get<node::Log>())
	{
		auto& src = static_cast<const node::Log&>(node);
		std::static_pointer_cast<sw::node::Log>(dst)->
			SetType(static_cast<sw::node::Log::BaseType>(src.GetType()));
	}
	else if (type == rttr::type::get<node::MatrixConstruction>())
	{
		auto& src = static_cast<const node::MatrixConstruction&>(node);
		std::static_pointer_cast<sw::node::MatrixConstruction>(dst)->
			SetRow(src.GetType() == MatrixType::ROW);
	}
	else if (type == rttr::type::get<node::MatrixSplit>())
	{
		auto& src = static_cast<const node::MatrixSplit&>(node);
		std::static_pointer_cast<sw::node::MatrixSplit>(dst)->
			SetRow(src.GetType() == MatrixType::ROW);
	}
	else if (type == rttr::type::get<node::Comparison>())
	{
		auto& src = static_cast<const node::Comparison&>(node);
		std::static_pointer_cast<sw::node::Comparison>(dst)->
			SetCmpType(static_cast<sw::node::Comparison::CmpType>(src.GetType()));
	}
	else if (type == rttr::type::get<node::Flipbook>())
	{
		auto& src = static_cast<const node::Flipbook&>(node);
		std::static_pointer_cast<sw::node::Flipbook>(dst)->
			SetInvert(src.GetInvert());
	}
	else if (type == rttr::type::get<node::Rotate>())
	{
		auto& src = static_cast<const node::Rotate&>(node);
		std::static_pointer_cast<sw::node::Rotate>(dst)->
			SetRadians(src.GetAngleType() == PropAngleType::RADIAN);
	}
    else if (type == rttr::type::get<node::Custom>())
    {
        auto& src = static_cast<const node::Custom&>(node);

        std::vector<sw::Variable> params;
        auto& inputs = src.GetAllInput();
        params.reserve(inputs.size());
        for (auto& i : inputs) {
            sw::Variable var(var_type_sg_to_sw(i->GetType()), i->GetName());
            params.push_back(var);
        }
        sw::NodeHelper::SetImports(*dst, params);

        auto& outputs = src.GetAllOutput();
        if (!outputs.empty())
        {
            sw::Variable var(var_type_sg_to_sw(outputs[0]->GetType()), outputs[0]->GetName());
            sw::NodeHelper::SetExports(*dst, { var });
        }

        auto cus = std::static_pointer_cast<sw::node::Custom>(dst);
        cus->SetName(src.GetName());
        cus->SetHeadStr(src.GetHeadStr());
        cus->SetBodyStr(src.GetBodyStr());
    }
    else if (type == rttr::type::get<bp::node::Function>())
    {
        auto& src = static_cast<const bp::node::Function&>(node);

        std::vector<sw::Variable> params;
        auto& inputs = src.GetAllInput();
        params.reserve(inputs.size());
        for (auto& i : inputs) {
            sw::Variable var(var_type_sg_to_sw(i->GetType()), i->GetName());
            params.push_back(var);
        }
        sw::NodeHelper::SetImports(*dst, params);

        auto& outputs = src.GetAllOutput();
        if (!outputs.empty())
        {
            sw::Variable var(var_type_sg_to_sw(outputs[0]->GetType()), outputs[0]->GetName());
            sw::NodeHelper::SetExports(*dst, { var });
        }

        dst->SetName(src.GetName());

        std::vector<sw::NodePtr> end_nodes;
        for (auto& c : src.GetChildren())
        {
            assert(c->HasUniqueComp<bp::CompNode>());
            auto bp_node = c->GetUniqueComp<bp::CompNode>().GetNode();
            if (bp_node->GetAllOutput().empty()) {
                end_nodes.push_back(CreateWeaverNode(*bp_node));
            }
        }
        auto func_node = std::static_pointer_cast<sw::node::Function>(dst);
        func_node->SetEndNodes(end_nodes);
    }
    else if (type == rttr::type::get<bp::node::Input>())
    {
		auto& src = static_cast<const bp::node::Input&>(node);
        std::static_pointer_cast<sw::node::FuncInput>(dst)->SetNameAndType(
            src.GetName(), var_type_sg_to_sw(src.GetType())
        );
    }
    else if (type == rttr::type::get<bp::node::Output>())
    {
		auto& src = static_cast<const bp::node::Output&>(node);
        std::static_pointer_cast<sw::node::FuncOutput>(dst)->SetNameAndType(
            src.GetName(), var_type_sg_to_sw(src.GetType())
        );
    }

    // connect
    for (int i = 0, n = node.GetAllInput().size(); i < n; ++i)
    {
        auto& imports = dst->GetImports();
        if (node.IsExtensibleInputPorts() && i >= static_cast<int>(imports.size())) {
            continue;
        }
        if (!imports[i].var.IsDefaultInput()) {
            continue;
        }
        sw::Node::PortAddr from_port;
        if (!CreateFromNode(node, i, from_port) ||
            from_port.node.expired()) {
            continue;
        }
        sw::make_connecting(from_port, { dst, i });
    }

    m_cached_nodes.push_back(dst);

	return dst;
}

void ShaderWeaver::PrepareSetRefNodes(const std::vector<bp::NodePtr>& all_nodes)
{
    for (auto& n : all_nodes) {
        if (n->get_type() == rttr::type::get<bp::node::SetReference>()) {
            auto set_node = std::static_pointer_cast<const bp::node::SetReference>(n);
            m_map2setnodes.insert({ set_node->GetName(), set_node });
        }
    }
}

bool ShaderWeaver::CreateFromNode(const bp::Node& node, int input_idx, sw::Node::PortAddr& from_port)
{
    auto& to_port = node.GetAllInput()[input_idx];
    auto& conns = to_port->GetConnecting();
    if (conns.empty()) {
        return false;
    }
    assert(conns.size() == 1);
    auto& bp_from_port = conns[0]->GetFrom();
    assert(bp_from_port);

    from_port.node = CreateWeaverNode(bp_from_port->GetParent());
    from_port.idx  = bp_from_port->GetPosIdx();
    return true;
}
//
//pt0::Shader::Params ShaderWeaver::CreateShaderParams(const sw::Evaluator& vert, const sw::Evaluator& frag) const
//{
//	pt0::Shader::Params sp(m_texture_names, m_layout);
//	sp.vs = vert.GenShaderStr().c_str();
//	sp.fs = frag.GenShaderStr().c_str();
//
//	sp.uniform_names.Add(pt0::UniformTypes::ModelMat, MODEL_MAT_NAME);
//	sp.uniform_names.Add(pt0::UniformTypes::ViewMat,  VIEW_MAT_NAME);
//	sp.uniform_names.Add(pt0::UniformTypes::ProjMat,  PROJ_MAT_NAME);
//
//	if (vert.HasNodeType<sw::node::Time>() || frag.HasNodeType<sw::node::Time>()) {
//        sp.uniform_names.Add(pt0::UniformTypes::Time,       sw::node::Time::TimeName());
//        sp.uniform_names.Add(pt0::UniformTypes::SineTime,  sw::node::Time::SineTimeName());
//        sp.uniform_names.Add(pt0::UniformTypes::CosTime,   sw::node::Time::CosTimeName());
//        sp.uniform_names.Add(pt0::UniformTypes::DeltaiTme, sw::node::Time::DeltaTimeName());
//	}
//
//    if (vert.HasNodeType<sw::node::Raymarching>() || frag.HasNodeType<sw::node::Raymarching>()) {
//        sp.uniform_names.Add(pt0::UniformTypes::Resolution, sw::node::Raymarching::ResolutionName());
//    }
//
//    if (vert.HasNodeType<sw::node::CameraPos>() || frag.HasNodeType<sw::node::CameraPos>()) {
//        sp.uniform_names.Add(pt0::UniformTypes::CamPos, sw::node::CameraPos::CamPosName());
//    }
//
//	return sp;
//}

}