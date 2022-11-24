#include <veda/internal/api.h>
#include <veda/cpp/api.h>

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
// CFunctionBase
//------------------------------------------------------------------------------
CFunctionBase::CFunctionBase(void) :
	VEDAfunction	{},
	m_ctx		(0)
{}

//------------------------------------------------------------------------------
CFunctionBase::CFunctionBase(Module mod, const std::string_view name) :
	VEDAfunction	(mod->getFunction(name)),
	m_ctx		(mod->ctx())
{}

//------------------------------------------------------------------------------
// CFunctionBase::Launcher
//------------------------------------------------------------------------------
Future::ReqId CFunctionBase::Launcher::launch(Args& args, Future::Ptr ptr) {
	return m_func.m_ctx->call(m_func, m_stream, args.consume(), true, false, ptr);
}

//------------------------------------------------------------------------------
	}
}