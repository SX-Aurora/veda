#include <veda/internal.h>

namespace veda {
//------------------------------------------------------------------------------
const Stream::Calls&	StreamGuard::calls	(void) const	{	return m_stream.calls;				}
int			StreamGuard::state	(void) const	{	return veo_get_context_state(m_stream.ctx);	}
void			StreamGuard::clear	(void) 		{	m_stream.calls.clear();				}

//------------------------------------------------------------------------------
StreamGuard::StreamGuard(Stream& s) : m_stream(s) {
	m_mutex.lock();
}

//------------------------------------------------------------------------------
StreamGuard::~StreamGuard(void) {
	m_mutex.unlock();
}

//------------------------------------------------------------------------------
void StreamGuard::enqueue(const uint64_t req, const bool checkResult, uint64_t* result) {
	m_stream.calls.emplace_back(req, checkResult, result);
}

//------------------------------------------------------------------------------
uint64_t StreamGuard::wait(const int id) const {
	uint64_t res = 0;
	TVEO(veo_call_wait_result(m_stream.ctx, id, &res));
	return res;
}

//------------------------------------------------------------------------------
void StreamGuard::enqueue(const bool checkResult, uint64_t* result, VEDAfunction func, VEDAargs args, const int idx) {
	enqueue(veo_call_async, checkResult, result, func, args);
	TVEDA(vedaArgsDestroy(args));
}

//------------------------------------------------------------------------------
}
