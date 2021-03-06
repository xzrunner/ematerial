#include "shaderlab/RegistNodes.h"
#include "shaderlab/node/Custom.h"
#include "shaderlab/node/StandardSurfaceOutput.h"
#include "shaderlab/node/Tex2DAsset.h"
#include "shaderlab/node/TexCubeAsset.h"

#include <ee0/ReflectPropTypes.h>
#include <blueprint/Pin.h>

#include <js/RTTR.h>

#define REGIST_NODE_RTTI(name, prop)                          \
	rttr::registration::class_<shaderlab::node::name>("shaderlab::"#name)   \
		.constructor<>()                                      \
		prop                                                  \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(name) REGIST_NODE_RTTI(name, PROP)

RTTR_REGISTRATION
{

// artistic
REGIST_NODE_RTTI_DEFAULT(Contrast)
REGIST_NODE_RTTI(Hue,                                                                  \
.property("angle_type", &shaderlab::node::Hue::GetAngleType, &shaderlab::node::Hue::SetAngleType)    \
(                                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PropAngleType"))             \
)
)
REGIST_NODE_RTTI(InvertColors,                                                                     \
.property("channels", &shaderlab::node::InvertColors::GetChannels, &shaderlab::node::InvertColors::SetChannels)  \
(                                                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Channels"))                              \
)
)
REGIST_NODE_RTTI_DEFAULT(ReplaceColor)
REGIST_NODE_RTTI_DEFAULT(Saturation)
REGIST_NODE_RTTI_DEFAULT(WhiteBalance)
REGIST_NODE_RTTI(Blend,                                                  \
.property("mode", &shaderlab::node::Blend::GetMode, &shaderlab::node::Blend::SetMode)  \
(                                                                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Mode"))        \
)
)
REGIST_NODE_RTTI_DEFAULT(EdgeDetect)
REGIST_NODE_RTTI_DEFAULT(Gray)
REGIST_NODE_RTTI_DEFAULT(MinFilter)
REGIST_NODE_RTTI(ChannelMask,                                                                   \
.property("channels", &shaderlab::node::ChannelMask::GetChannels, &shaderlab::node::ChannelMask::SetChannels) \
(                                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Channels"))                           \
)
)
REGIST_NODE_RTTI_DEFAULT(ColorMask)
REGIST_NODE_RTTI_DEFAULT(NormalBlend)
REGIST_NODE_RTTI_DEFAULT(NormalCreate)
REGIST_NODE_RTTI_DEFAULT(NormalStrength)
REGIST_NODE_RTTI_DEFAULT(NormalUnpack)
REGIST_NODE_RTTI_DEFAULT(ColorAddMul)
REGIST_NODE_RTTI_DEFAULT(ColorMap)
REGIST_NODE_RTTI(ColorspaceConversion,                                                        \
.property("type", &shaderlab::node::ColorspaceConversion::GetType, &shaderlab::node::ColorspaceConversion::SetType) \
(                                                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ColTrans"))                                 \
)
)
REGIST_NODE_RTTI_DEFAULT(RGBToHSV)
REGIST_NODE_RTTI_DEFAULT(HSVToRGB)
// channel
REGIST_NODE_RTTI_DEFAULT(Combine)
REGIST_NODE_RTTI(Flip,                                                            \
.property("channels", &shaderlab::node::Flip::GetChannels, &shaderlab::node::Flip::SetChannels) \
(                                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Channels"))             \
)
)
REGIST_NODE_RTTI_DEFAULT(Split)
REGIST_NODE_RTTI(Swizzle,                                                               \
.property("channels", &shaderlab::node::Swizzle::GetChannels, &shaderlab::node::Swizzle::SetChannels) \
(                                                                                       \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Channels"))                   \
)
)
// input
REGIST_NODE_RTTI(Boolean,                                                    \
.property("val", &shaderlab::node::Boolean::GetValue, &shaderlab::node::Boolean::SetValue) \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vector1B"))        \
)
)
REGIST_NODE_RTTI_DEFAULT(Time)
REGIST_NODE_RTTI(Vector1,                                                  \
.property("x", &shaderlab::node::Vector1::GetValue, &shaderlab::node::Vector1::SetValue) \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vector1"))       \
)
)
REGIST_NODE_RTTI(Vector2,                                                    \
.property("x", &shaderlab::node::Vector2::GetX, &shaderlab::node::Vector2::SetX)           \
.property("y", &shaderlab::node::Vector2::GetY, &shaderlab::node::Vector2::SetY)           \
.property("val", &shaderlab::node::Vector2::GetValue, &shaderlab::node::Vector2::SetValue) \
(                                                                            \
	rttr::metadata(js::RTTR::NoSerializeTag(), true),                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vector2"))         \
)
)
REGIST_NODE_RTTI(Vector3,                                                    \
.property("x", &shaderlab::node::Vector3::GetX, &shaderlab::node::Vector3::SetX)           \
.property("y", &shaderlab::node::Vector3::GetY, &shaderlab::node::Vector3::SetY)           \
.property("z", &shaderlab::node::Vector3::GetZ, &shaderlab::node::Vector3::SetZ)           \
.property("val", &shaderlab::node::Vector3::GetValue, &shaderlab::node::Vector3::SetValue) \
(                                                                            \
	rttr::metadata(js::RTTR::NoSerializeTag(), true),                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vector3"))         \
)
)
REGIST_NODE_RTTI(Vector4,                                                    \
.property("x", &shaderlab::node::Vector4::GetX, &shaderlab::node::Vector4::SetX)           \
.property("y", &shaderlab::node::Vector4::GetY, &shaderlab::node::Vector4::SetY)           \
.property("z", &shaderlab::node::Vector4::GetZ, &shaderlab::node::Vector4::SetZ)           \
.property("w", &shaderlab::node::Vector4::GetW, &shaderlab::node::Vector4::SetW)           \
.property("val", &shaderlab::node::Vector4::GetValue, &shaderlab::node::Vector4::SetValue) \
(                                                                            \
	rttr::metadata(js::RTTR::NoSerializeTag(), true),                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vector4"))         \
)
)
REGIST_NODE_RTTI_DEFAULT(CameraPos)
REGIST_NODE_RTTI_DEFAULT(UV)
REGIST_NODE_RTTI(ViewDirection,                                                                                     \
.property("view_space", &shaderlab::node::ViewDirection::GetViewSpace, &shaderlab::node::ViewDirection::SetViewSpace)             \
(                                                                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ViewSpace"))                                              \
)                                                                                                                   \
.property("safe_normalize", &shaderlab::node::ViewDirection::GetSafeNormalize, &shaderlab::node::ViewDirection::SetSafeNormalize) \
(                                                                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SafeNormalize"))                                          \
)
)
REGIST_NODE_RTTI_DEFAULT(IndirectDiffuseLighting)
REGIST_NODE_RTTI_DEFAULT(IndirectSpecularLight)
REGIST_NODE_RTTI_DEFAULT(LightAttenuation)
REGIST_NODE_RTTI_DEFAULT(LightColor)
REGIST_NODE_RTTI_DEFAULT(WorldSpaceLightDir)
REGIST_NODE_RTTI(Matrix2,                                                    \
.property("mat", &shaderlab::node::Matrix2::GetValue, &shaderlab::node::Matrix2::SetValue)
)
REGIST_NODE_RTTI(Matrix3,                                                    \
.property("mat", &shaderlab::node::Matrix3::GetValue, &shaderlab::node::Matrix3::SetValue)
)
REGIST_NODE_RTTI(Matrix4,                                                    \
.property("mat", &shaderlab::node::Matrix4::GetValue, &shaderlab::node::Matrix4::SetValue)
)
REGIST_NODE_RTTI_DEFAULT(Color)
REGIST_NODE_RTTI_DEFAULT(PI)
REGIST_NODE_RTTI_DEFAULT(WorldBitangent)
REGIST_NODE_RTTI_DEFAULT(WorldPosition)
REGIST_NODE_RTTI_DEFAULT(WorldTangent)
REGIST_NODE_RTTI_DEFAULT(VertexBitangent)
REGIST_NODE_RTTI_DEFAULT(VertexNormal)
REGIST_NODE_RTTI_DEFAULT(VertexTangent)
REGIST_NODE_RTTI_DEFAULT(SampleTex2D)
REGIST_NODE_RTTI_DEFAULT(SampleTex3D)
REGIST_NODE_RTTI_DEFAULT(SampleTriplanar)
REGIST_NODE_RTTI_DEFAULT(UnpackScaleNormal)
REGIST_NODE_RTTI(Tex2DAsset,                                                                    \
.property("name", &shaderlab::node::Tex2DAsset::GetName, &shaderlab::node::Tex2DAsset::SetName)               \
(                                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                               \
)                                                                                               \
.property("filepath", &shaderlab::node::Tex2DAsset::GetImagePath, &shaderlab::node::Tex2DAsset::SetImagePath) \
(                                                                                               \
	rttr::metadata(js::RTTR::FilePathTag(), true),                                              \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),                          \
	rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.png"))                          \
)
)
REGIST_NODE_RTTI(TexCubeAsset,                                                                      \
.property("name", &shaderlab::node::TexCubeAsset::GetName, &shaderlab::node::TexCubeAsset::SetName)               \
(                                                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))                                   \
)                                                                                                   \
.property("filepath", &shaderlab::node::TexCubeAsset::GetImagePath, &shaderlab::node::TexCubeAsset::SetImagePath) \
(                                                                                                   \
	rttr::metadata(js::RTTR::FilePathTag(), true),                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),                              \
	rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.png"))                              \
)
)
// master
REGIST_NODE_RTTI_DEFAULT(PBR)
REGIST_NODE_RTTI_DEFAULT(Phong)
REGIST_NODE_RTTI_DEFAULT(Raymarching)
REGIST_NODE_RTTI_DEFAULT(Sprite)
// math
REGIST_NODE_RTTI_DEFAULT(Absolute)
REGIST_NODE_RTTI(Exponential,                                                       \
.property("type", &shaderlab::node::Exponential::GetType, &shaderlab::node::Exponential::SetType) \
(                                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Base"))                   \
)
)
REGIST_NODE_RTTI_DEFAULT(Length)
REGIST_NODE_RTTI(Log,                                               \
.property("type", &shaderlab::node::Log::GetType, &shaderlab::node::Log::SetType) \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Base"))   \
)
)
REGIST_NODE_RTTI_DEFAULT(Modulo)
REGIST_NODE_RTTI_DEFAULT(Negate)
REGIST_NODE_RTTI_DEFAULT(Normalize)
REGIST_NODE_RTTI_DEFAULT(Posterize)
REGIST_NODE_RTTI_DEFAULT(Reciprocal)
REGIST_NODE_RTTI_DEFAULT(ReciprocalSquareRoot)
REGIST_NODE_RTTI_DEFAULT(Add)
REGIST_NODE_RTTI_DEFAULT(Divide)
REGIST_NODE_RTTI_DEFAULT(Multiply)
REGIST_NODE_RTTI_DEFAULT(Power)
REGIST_NODE_RTTI_DEFAULT(SquareRoot)
REGIST_NODE_RTTI_DEFAULT(Subtract)
REGIST_NODE_RTTI_DEFAULT(DDX)
REGIST_NODE_RTTI_DEFAULT(DDXY)
REGIST_NODE_RTTI_DEFAULT(DDY)
REGIST_NODE_RTTI_DEFAULT(FWidth)
REGIST_NODE_RTTI_DEFAULT(InverseLerp)
REGIST_NODE_RTTI_DEFAULT(Lerp)
REGIST_NODE_RTTI_DEFAULT(Smoothstep)
REGIST_NODE_RTTI(MatrixConstruction,                                                              \
.property("type", &shaderlab::node::MatrixConstruction::GetType, &shaderlab::node::MatrixConstruction::SetType) \
(                                                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))                                 \
)
)
REGIST_NODE_RTTI_DEFAULT(MatrixDeterminant)
REGIST_NODE_RTTI_DEFAULT(MatrixInverse)
REGIST_NODE_RTTI(MatrixSplit,                                                                     \
.property("type", &shaderlab::node::MatrixConstruction::GetType, &shaderlab::node::MatrixConstruction::SetType) \
(                                                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))                                 \
)
)
REGIST_NODE_RTTI_DEFAULT(MatrixTranspose)
REGIST_NODE_RTTI_DEFAULT(Clamp)
REGIST_NODE_RTTI_DEFAULT(Fraction)
REGIST_NODE_RTTI_DEFAULT(Maximum)
REGIST_NODE_RTTI_DEFAULT(Minimum)
REGIST_NODE_RTTI_DEFAULT(OneMinus)
REGIST_NODE_RTTI_DEFAULT(RandomRange)
REGIST_NODE_RTTI_DEFAULT(Remap)
REGIST_NODE_RTTI_DEFAULT(Saturate)
REGIST_NODE_RTTI_DEFAULT(Ceiling)
REGIST_NODE_RTTI_DEFAULT(Floor)
REGIST_NODE_RTTI_DEFAULT(Round)
REGIST_NODE_RTTI_DEFAULT(Sign)
REGIST_NODE_RTTI_DEFAULT(Step)
REGIST_NODE_RTTI_DEFAULT(Truncate)
REGIST_NODE_RTTI_DEFAULT(Arccosine)
REGIST_NODE_RTTI_DEFAULT(Arcsine)
REGIST_NODE_RTTI_DEFAULT(Arctangent)
REGIST_NODE_RTTI_DEFAULT(Arctangent2)
REGIST_NODE_RTTI_DEFAULT(Cosine)
REGIST_NODE_RTTI_DEFAULT(DegreesToRadians)
REGIST_NODE_RTTI_DEFAULT(HyperbolicCosine)
REGIST_NODE_RTTI_DEFAULT(HyperbolicSine)
REGIST_NODE_RTTI_DEFAULT(HyperbolicTangent)
REGIST_NODE_RTTI_DEFAULT(RadiansToDegrees)
REGIST_NODE_RTTI_DEFAULT(Sine)
REGIST_NODE_RTTI_DEFAULT(Tangent)
REGIST_NODE_RTTI_DEFAULT(CrossProduct)
REGIST_NODE_RTTI_DEFAULT(Distance)
REGIST_NODE_RTTI_DEFAULT(DotProduct)
REGIST_NODE_RTTI_DEFAULT(Projection)
REGIST_NODE_RTTI_DEFAULT(Rejection)
REGIST_NODE_RTTI_DEFAULT(TransformDirection)
REGIST_NODE_RTTI_DEFAULT(ScaleAndOffset)
// procedural
REGIST_NODE_RTTI_DEFAULT(Checkerboard)
REGIST_NODE_RTTI_DEFAULT(Ellipse)
REGIST_NODE_RTTI_DEFAULT(GradientNoise)
REGIST_NODE_RTTI_DEFAULT(Polygon)
REGIST_NODE_RTTI_DEFAULT(Rectangle)
REGIST_NODE_RTTI_DEFAULT(RoundedRectangle)
REGIST_NODE_RTTI_DEFAULT(SimpleNoise)
REGIST_NODE_RTTI_DEFAULT(Voronoi)
// sdf
REGIST_NODE_RTTI_DEFAULT(Intersection)
REGIST_NODE_RTTI_DEFAULT(Subtraction)
REGIST_NODE_RTTI_DEFAULT(Union)
REGIST_NODE_RTTI_DEFAULT(SDF)
REGIST_NODE_RTTI_DEFAULT(Sphere)
REGIST_NODE_RTTI_DEFAULT(Torus)
REGIST_NODE_RTTI_DEFAULT(Box)
REGIST_NODE_RTTI_DEFAULT(EstimateNormal)
REGIST_NODE_RTTI_DEFAULT(PhongIllumination)
// tools
REGIST_NODE_RTTI_DEFAULT(Relay)
// utility
REGIST_NODE_RTTI_DEFAULT(And)
REGIST_NODE_RTTI_DEFAULT(Branch)
REGIST_NODE_RTTI(Comparison,                                      \
(                                                                 \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type")) \
)
)
REGIST_NODE_RTTI_DEFAULT(Not)
REGIST_NODE_RTTI_DEFAULT(Or)
REGIST_NODE_RTTI_DEFAULT(ViewMatrix)
REGIST_NODE_RTTI_DEFAULT(WorldNormalVector)
REGIST_NODE_RTTI_DEFAULT(DecodeFloatRGBA)
REGIST_NODE_RTTI_DEFAULT(EncodeFloatRGBA)
REGIST_NODE_RTTI_DEFAULT(VertexToFragment)
// uv
REGIST_NODE_RTTI(Flipbook,                                                          \
.property("invert", &shaderlab::node::Flipbook::GetInvert, &shaderlab::node::Flipbook::SetInvert) \
(                                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Invert"))                 \
)
)
REGIST_NODE_RTTI_DEFAULT(PolarCoordinates)
REGIST_NODE_RTTI_DEFAULT(RadialShear)
REGIST_NODE_RTTI(Rotate,                                                               \
.property("radians", &shaderlab::node::Rotate::GetAngleType, &shaderlab::node::Rotate::SetAngleType) \
(                                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PropAngleType"))             \
)
)
REGIST_NODE_RTTI_DEFAULT(Spherize)
REGIST_NODE_RTTI_DEFAULT(TilingAndOffset)
REGIST_NODE_RTTI_DEFAULT(Twirl)
// tools
REGIST_NODE_RTTI(Custom,                                                                    \
.property("head", &shaderlab::node::Custom::GetHeadStr, &shaderlab::node::Custom::SetHeadStr)             \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("HeadStr")),                       \
    rttr::metadata(ee0::PropLongStringTag(), true)                                          \
)                                                                                           \
.property("body", &shaderlab::node::Custom::GetBodyStr, &shaderlab::node::Custom::SetBodyStr)             \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BodyStr")),                       \
    rttr::metadata(ee0::PropLongStringTag(), true)                                          \
)                                                                                           \
.property("internal", &shaderlab::node::Custom::GetInternalStr, &shaderlab::node::Custom::SetInternalStr) \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("InternalStr")),                   \
    rttr::metadata(ee0::PropLongStringTag(), true)                                          \
)
)
// anim
REGIST_NODE_RTTI_DEFAULT(Skin)
// master
REGIST_NODE_RTTI_DEFAULT(StandardSurfaceOutput)

}

namespace shaderlab
{

void nodes_regist_rttr()
{
}

namespace node
{

//////////////////////////////////////////////////////////////////////////
// class ChannelMask
//////////////////////////////////////////////////////////////////////////

void ChannelMask::SetChannels(const PropMultiChannels& channels)
{
    m_channels = channels;

    int dim = 0;
    if (channels.channels & PropMultiChannels::CHANNEL_R) {
        ++dim;
    }
    if (channels.channels & PropMultiChannels::CHANNEL_G) {
        ++dim;
    }
    if (channels.channels & PropMultiChannels::CHANNEL_B) {
        ++dim;
    }
    if (channels.channels & PropMultiChannels::CHANNEL_A) {
        ++dim;
    }
    int type = 0;
    switch (dim)
    {
    case 0:
        type = bp::PIN_ANY_VAR;
        break;
    case 1:
        type = PIN_VECTOR1;
        break;
    case 2:
        type = PIN_VECTOR2;
        break;
    case 3:
        type = PIN_VECTOR3;
        break;
    case 4:
        type = PIN_VECTOR4;
        break;
    default:
        assert(0);
    }
    assert(m_all_output.size() == 1);
    m_all_output[0]->SetType(type);
    m_all_output[0]->SetOldType(type);
}

}
}