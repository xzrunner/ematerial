#include "shadergraph/PhongModel.h"
#include "shadergraph/Utility.h"
#include "shadergraph/TextureObject.h"
#include "shadergraph/PinsType.h"

#include <blueprint/Connecting.h>
#include <painting2/Texture.h>
#include <painting3/Material.h>
#include <facade/Image.h>

namespace shadergraph
{

const std::string PhongModel::TYPE_NAME = "mat_phong_model";

PhongModel::PhongModel()
	: Node("PhongModel")
{
	AddPins(m_ambient = std::make_shared<bp::node::Pins>(
		true, ID_AMBIENT, PINS_VECTOR3, "ambient", *this));
	AddPins(m_diffuse = std::make_shared<bp::node::Pins>(
		true, ID_DIFFUSE, PINS_VECTOR3, "diffuse", *this));
	AddPins(m_specular = std::make_shared<bp::node::Pins>(
		true, ID_SPECULAR, PINS_VECTOR3, "specular", *this));
	AddPins(m_shininess = std::make_shared<bp::node::Pins>(
		true, ID_SHININESS, PINS_VECTOR1, "shininess", *this));
	AddPins(m_diffuse_tex = std::make_shared<bp::node::Pins>(
		true, ID_DIFFUSE_TEX, PINS_TEXTURE2D, "diffuse_tex", *this));

	Layout();
}

void PhongModel::CalcMaterial(pt3::Material& mat) const
{
	mat.ambient   = Utility::CalcNodeInputVal(*m_ambient);
	mat.diffuse   = Utility::CalcNodeInputVal(*m_diffuse);
	mat.specular  = Utility::CalcNodeInputVal(*m_specular);
	mat.shininess = Utility::CalcNodeInputVal(*m_shininess).x;

	auto& d_tex_conn = m_diffuse_tex->GetConnecting();
	if (d_tex_conn.size() == 1)
	{
		auto from = d_tex_conn[0]->GetFrom();
		if (from) {
			auto& to = dynamic_cast<const TextureObject&>(from->GetParent());
			if (auto& img = to.GetImage()) {
				mat.diffuse_tex = std::const_pointer_cast<pt2::Texture>(img->GetTexture());
			}
		}
	}
}

}