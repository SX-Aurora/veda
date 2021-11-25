#include "veda.hpp"

namespace veda {
//------------------------------------------------------------------------------
thread_local std::list<VEDAcontext> t_stack;

//------------------------------------------------------------------------------
VEDAcontext Contexts::current(void) {
	if(t_stack.empty())
		VEDA_THROW(VEDA_ERROR_UNKNOWN_CONTEXT);
	return t_stack.back();
}

//------------------------------------------------------------------------------
VEDAcontext Contexts::pop(void) {
	auto ctx = current();
	t_stack.pop_back();
	return ctx;
}

//------------------------------------------------------------------------------
void Contexts::push(VEDAcontext ctx) {
	t_stack.emplace_back(ctx);
}

//------------------------------------------------------------------------------
void Contexts::set(VEDAcontext ctx) {
	if(ctx) {
		if(t_stack.empty())	t_stack.emplace_back(ctx);
		else			t_stack.back() = ctx;
	} else {
		if(!t_stack.empty())	t_stack.pop_back();
	}
}

//------------------------------------------------------------------------------
}