#include "veda.h"

namespace veda {
//------------------------------------------------------------------------------
thread_local std::list<VEDAcontext> t_stack;

//------------------------------------------------------------------------------
VEDAcontext Contexts::current(void) {
	if(t_stack.empty())
		throw VEDA_ERROR_UNKNOWN_CONTEXT;
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
}