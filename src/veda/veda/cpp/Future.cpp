#include <veda/internal/api.h>
#include <veda/cpp/api.h>

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
void Future::wait(void) {
	if(m_ctx) {
		m_ctx->sync(m_stream, m_until);
		m_ctx = 0;
	}
}

//------------------------------------------------------------------------------
	}
}