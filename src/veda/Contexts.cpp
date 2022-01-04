#include "veda.hpp"

namespace veda {
//------------------------------------------------------------------------------
thread_local std::list<VEDAcontext> t_stack;

//------------------------------------------------------------------------------
VEDAcontext Contexts::current(void) {
	if(t_stack.empty())
		VEDA_THROW(VEDA_ERROR_UNKNOWN_CONTEXT);
	auto ctx = t_stack.back();
	if(!ctx->isActive())
		VEDA_THROW(VEDA_ERROR_CONTEXT_IS_DESTROYED);
	return ctx;
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
void Contexts::remove(VEDAcontext ctx) {
	t_stack.remove(ctx);
}

//------------------------------------------------------------------------------
void Contexts::shutdown(void) {
	t_stack.clear();
}

//------------------------------------------------------------------------------
int Contexts::countInstances(VEDAcontext ctx) {
	int count = 0;
	for(auto it : t_stack)
		if(it == ctx)
			count++;
	return count;
}

//------------------------------------------------------------------------------
void Contexts::set(VEDAcontext ctx) {
	if(ctx) {
		if(t_stack.empty())	t_stack.emplace_back(ctx);
		else 			t_stack.back() = ctx;
	} else {
		if(!t_stack.empty()) 
			t_stack.pop_back();
	}
}

//------------------------------------------------------------------------------
}