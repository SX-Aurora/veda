#include <veda/internal/api.h>

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
Stream::Stream(veo_thr_ctxt* ctx, const int deviceId, const int streamId) :
	m_deviceId(deviceId),
	m_streamId(streamId),
	m_ctx(ctx)
{
	// Create a new AVEO context, a pseudo thread and corresponding
	// VE thread for the context.
	VEDA_ASSERT(m_ctx, VEDA_ERROR_CANNOT_CREATE_STREAM);
}

//------------------------------------------------------------------------------
void Stream::sync(const ReqId until) {
	for(auto it = m_calls.begin(); it != m_calls.end(); it = m_calls.erase(it)) {
		auto&& [id, checkResult, result] = *it;
		if(id > until)
			return;

		uint64_t res = 0;
		TVEO(veo_call_wait_result(m_ctx, id, &res));

		if(result)
			*result.get() = res;
		
		if(checkResult)
			VEDA_THROW_IF((VEDAresult)res);
	}
}

//------------------------------------------------------------------------------
ReqId Stream::enqueue(const uint64_t req, const bool checkResult, ResultPtr result) {
	m_calls.emplace_back(req, checkResult, result);
	return req;
}

//------------------------------------------------------------------------------
	}
}
