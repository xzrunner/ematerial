#include <ee0/MsgHelper.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>

#include <node0/CompIdentity.h>
#include <node0/SceneNode.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <moon/runtime.h>
#include <moon/Blackboard.h>
#include <moon/Context.h>
#include <moon/Blackboard.h>
#include <moon/SceneNode.h>
#include <moon/runtime.h>

namespace
{

class ShaderLab : public moon::Module
{
public:
	virtual moon::ModuleTypeID TypeID() const override {
		return moon::GetModuleTypeID<ShaderLab>();
	}
	virtual const char* GetName() const override {
		return "moon.sg";
	}

}; // ShaderLab

#define INSTANCE() (moon::Blackboard::Instance()->GetContext()->GetModuleMgr().GetModule<ShaderLab>())

moon::SceneNode* luax_checknode(lua_State* L, int idx)
{
	return moon::luax_checktype<moon::SceneNode>(L, idx, moon::SCENE_NODE_ID);
}

int w_new_node(lua_State* L)
{
	auto bb = moon::Blackboard::Instance();

	const char* type = luaL_checkstring(L, 1);
	auto rt_type = rttr::type::get_by_name(type);
	assert(rt_type.is_valid());
	auto rt_obj = rt_type.create();
	assert(rt_obj.is_valid());
	auto bp_node = rt_obj.get_value<bp::NodePtr>();
	if (!bp_node) {
		luaL_error(L, "fail to create node %s\n", type);
	}

	auto node = std::make_shared<n0::SceneNode>();
	auto& cnode = node->AddUniqueComp<bp::CompNode>();
	cnode.SetNode(bp_node);
	node->AddUniqueComp<n2::CompTransform>();
	node->AddUniqueComp<n0::CompIdentity>();
	auto& style = bp_node->GetStyle();
	node->AddUniqueComp<n2::CompBoundingBox>(
		sm::rect(style.width, style.height)
	);

	moon::SceneNode* moon_node = nullptr;
	moon::luax_catchexcept(L, [&]() {
		moon_node = new moon::SceneNode(node);
	});
	moon::luax_pushtype(L, moon::SCENE_NODE_ID, moon_node);
	moon_node->Release();

#ifdef EASYEDITOR
	ee0::MsgHelper::InsertNode(*bb->GetSubMgr(), node);
#endif // EASYEDITOR

	return 1;
}

int w_set_pos(lua_State* L)
{
	auto node = luax_checknode(L, 1);
	float x = static_cast<float>(luaL_optnumber(L, 2, 0));
	float y = static_cast<float>(luaL_optnumber(L, 3, 0));
	node->SetPosition(sm::vec2(x, y));
	return 0;
}

}

namespace moon
{

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "new_node", w_new_node },
	{ "set_pos", w_set_pos },

	{ 0, 0 }
};

// Types for this module.
static const lua_CFunction types[] =
{
	0
};

extern "C" int luaopen_moon_shaderlab(lua_State* L)
{
	ShaderLab* instance = INSTANCE();
	if (instance == nullptr) {
		luax_catchexcept(L, [&](){ instance = new ShaderLab(); });
	} else {
		instance->Retain();
	}

	WrappedModule w;
	w.module = instance;
	w.name = "sg";
	w.type = MODULE_ID;
	w.functions = functions;
	w.types = types;

	return luax_register_module(L, w);
}

}