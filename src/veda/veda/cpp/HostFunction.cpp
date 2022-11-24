#include <veda/internal/api.h>
#include <veda/cpp/api.h>

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
HostFunctionBase::HostFunctionBase(const veda::Device& dev) :
	m_ctx(dev.ctx())
{}

//------------------------------------------------------------------------------
Future::ReqId HostFunctionBase::launch(const StreamId stream, VEDAhost_function func, void* data, Future::Ptr ptr) const {
	return m_ctx->call(func, stream, data, false, ptr);
}

//------------------------------------------------------------------------------
	}
}