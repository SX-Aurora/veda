#include "veda/internal.h"

namespace veda {
//------------------------------------------------------------------------------
class Stacks;
struct Stack : public std::list<VEDAcontext> {
	Stacks*	m_stacks;
	
	Stack	(void);
	~Stack	(void);
};

//------------------------------------------------------------------------------
class Stacks : public std::set<Stack*> {
	std::mutex m_mutex;

public:
		~Stacks	(void);
	void	add	(Stack* stack);
	void	remove	(Stack* stack);
	void	clear	(void);
};

//------------------------------------------------------------------------------
// Stacks
//------------------------------------------------------------------------------
Stacks::~Stacks(void) {
	std::lock_guard<std::mutex> lock(m_mutex);
	for(auto stack : *this)
		stack->m_stacks = 0;
	std::set<Stack*>::clear();
}

//------------------------------------------------------------------------------
inline void Stacks::add(Stack* stack) {
	std::lock_guard<std::mutex> lock(m_mutex);
	emplace(stack);
	stack->m_stacks = this;
}

//------------------------------------------------------------------------------
inline void Stacks::remove(Stack* stack) {
	std::lock_guard<std::mutex> lock(m_mutex);
	erase(stack);
	stack->m_stacks = 0;
}

//------------------------------------------------------------------------------
inline void Stacks::clear(void) {
	std::lock_guard<std::mutex> lock(m_mutex);
	for(auto&& s : *this)
		s->clear();
}

//------------------------------------------------------------------------------
static Stacks s_stacks;

//------------------------------------------------------------------------------
// Stack
//------------------------------------------------------------------------------
inline Stack::Stack(void) : m_stacks(0) {
	s_stacks.add(this);
}

//------------------------------------------------------------------------------
inline Stack::~Stack(void) {
	if(m_stacks)
		m_stacks->remove(this);
}

//------------------------------------------------------------------------------
thread_local Stack t_stack;

//------------------------------------------------------------------------------
// Contexts
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
void Contexts::shutdown(void) {
	s_stacks.clear();
}

//------------------------------------------------------------------------------
}