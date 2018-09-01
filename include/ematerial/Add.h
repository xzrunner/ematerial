#pragma once

#include "ematerial/Node.h"

namespace ematerial
{

class Add : public Node
{
public:
	Add();

	virtual bp::NodeTypeID TypeID() const override {
		return bp::GetNodeTypeID<Add>();
	}
	virtual const std::string& TypeName() const override {
		return TYPE_NAME;
	}
	virtual std::shared_ptr<bp::node::Node> Create() const override {
		return std::make_shared<Add>();
	}

	virtual sm::vec3 ToVec3() const override;

	static const std::string TYPE_NAME;

private:
	std::shared_ptr<bp::node::Pins> m_input0;
	std::shared_ptr<bp::node::Pins> m_input1;

	std::shared_ptr<bp::node::Pins> m_output;

}; // Add

}