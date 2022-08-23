#include <veda/internal.h>

namespace veda {
//------------------------------------------------------------------------------
const Stream::Calls&	Stream::calls	(void) const	{	return m_calls;				}
int			Stream::state	(void) const	{	return veo_get_context_state(m_ctx);	}

//------------------------------------------------------------------------------
Stream::Stream(veo_thr_ctxt* ctx) : m_ctx(ctx) {
	// Create a new AVEO context, a pseudo thread and corresponding
	// VE thread for the context.
	VEDA_ASSERT(m_ctx, VEDA_ERROR_CANNOT_CREATE_STREAM);
	m_calls.reserve(128);
	ASSERT(m_calls.empty());	
}

//------------------------------------------------------------------------------
void Stream::lock(void) {
	veo_req_block_begin(m_ctx);
}

//------------------------------------------------------------------------------
void Stream::unlock(void) {
	veo_req_block_end(m_ctx);
}

//------------------------------------------------------------------------------
void Stream::enqueue(const uint64_t req, const bool checkResult, uint64_t* result) {
	m_calls.emplace_back(req, checkResult, result);
}

//------------------------------------------------------------------------------
uint64_t Stream::wait(const uint64_t id) const {
	uint64_t res = 0;
	TVEO(veo_call_wait_result(m_ctx, id, &res));
	return res;
}

//------------------------------------------------------------------------------
void Stream::enqueue(const bool checkResult, uint64_t* result, VEDAfunction func, VEDAargs args, const int idx) {
	enqueue(veo_call_async, checkResult, result, func, args);
	TVEDA(vedaArgsDestroy(args));
}

//------------------------------------------------------------------------------
void Stream::sync(void) {
	for(auto&& [id, checkResult, result] : m_calls) {
		auto res = wait(id);

		if(result)
			*result = res;
		
		if(checkResult) {
			auto veda = (VEDAresult)res;
			VEDA_ASSERT(veda == VEDA_SUCCESS, veda);
		}
	}
	
	m_calls.clear();
}

//------------------------------------------------------------------------------
}
