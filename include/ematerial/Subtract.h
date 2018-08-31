#pragma once

#include "ematerial/Node.h"

namespace ematerial
{

class Subtract : public Node
{
public:
	Subtract();

	virtual bp::NodeTypeID TypeID() const override {
		return bp::GetNodeTypeID<Subtract>();
	}
	virtual std::string TypeName() const override {
		return "mat_sub";
	}
	virtual std::shared_ptr<bp::node::Node> Create() const override {
		return std::make_shared<Subtract>();
	}

	virtual sm::vec3 ToVec3() const override;

private:
	std::shared_ptr<bp::node::Pins> m_input0;
	std::shared_ptr<bp::node::Pins> m_input1;

	std::shared_ptr<bp::node::Pins> m_output;

}; // Subtract

}