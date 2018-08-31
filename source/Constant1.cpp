#include "ematerial/Constant1.h"

#include <blueprint/Pins.h>
#include <cpputil/StringHelper.h>

namespace ematerial
{

Constant1::Constant1()
	: Node("Constant1")
{
	AddPins(m_output = std::make_shared<bp::node::Pins>(
		false, 0, bp::node::PINS_VECTOR, "", *this));

	Layout();

	UpdateTitle();
}

sm::vec3 Constant1::ToVec3() const
{
	return sm::vec3(m_val, m_val, m_val);
}

void Constant1::SetValue(float val)
{
	m_val = val;
	UpdateTitle();
}

void Constant1::UpdateTitle()
{
	m_style.small_title = true;
	m_title = cpputil::StringHelper::ToString(m_val, 2);
}

}