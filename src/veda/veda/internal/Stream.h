#pragma once

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
struct Stream {
	using Calls	= std::list<std::tuple<ReqId, bool, ResultPtr>>;

private:
	const int	m_deviceId;
	const int	m_streamId;
	veo_thr_ctxt*	m_ctx;
	Calls		m_calls;

	ReqId		enqueue		(const uint64_t req, const bool checkResult, ResultPtr result);
	
	template<VEDAprofiler_event event>
	inline ReqId enqueueProfiler(void* data) {
		assert(data);
		
		if constexpr (event == VEDA_PROFILER_EVENT_ISSUE) {
			profiler::callbackIssue(data);
			return -1;
		}

		return enqueue(veo_call_async_vh(m_ctx, event == VEDA_PROFILER_EVENT_BEGIN ? profiler::callbackBegin : profiler::callbackEnd, data), false, {});
	}

public:
		Stream	(veo_thr_ctxt* ctx, const int deviceId, const int streamId);
	void	sync	(const ReqId until = -1);

	inline const Calls&	calls	(void) const	{ return m_calls;			}
	inline int		state	(void) const	{ return veo_get_context_state(m_ctx);	}
	inline void		lock	(void)		{ veo_req_block_begin(m_ctx);		}
	inline void		unlock	(void)		{ veo_req_block_end(m_ctx);		}

	template<typename F, typename... Args>
	inline typename std::enable_if<is_function_ptr<F>::value, ReqId>::type enqueue(F func, const bool checkResult, const bool async, ResultPtr result, const char* kernel, Args... args) {
		if(auto data = profiler::data(m_deviceId, m_streamId, async, func, args..., kernel)) {
			// in case the user didn't provide a result, we use the one of data
			enqueueProfiler<VEDA_PROFILER_EVENT_ISSUE>(data);
			enqueueProfiler<VEDA_PROFILER_EVENT_BEGIN>(data);
			data->req_id = CREQ(func(m_ctx, args...));
			enqueue		(data->req_id, checkResult, result);
			return enqueueProfiler<VEDA_PROFILER_EVENT_END>(data);
		} else {
			return enqueue(CREQ(func(m_ctx, args...)), checkResult, result);
		}
	}

	inline ReqId enqueue(const bool checkResult, const bool async, ResultPtr result, const VEDAfunction& func, VEDAargs args, const int idx) {
		auto id = enqueue(veo_call_async, checkResult, async, result, func.kernel, func.ptr, args);
		TVEDA(vedaArgsDestroy(args));
		return id;
	}

	template<typename T, typename... Args>
	inline ReqId enqueue(const bool checkResult, const bool async, ResultPtr result, const VEDAfunction& func, VEDAargs args, const int idx, const T value, Args... vargs) {
		TVEDA(vedaArgsSet(args, idx, value));
		return enqueue(checkResult, async, result, func, args, idx+1, vargs...);
	}

	template<typename... Args>
	inline ReqId enqueue(const bool checkResult, const bool async, ResultPtr result, const VEDAfunction& func, Args... vargs) {
		VEDAargs args = 0;
		TVEDA(vedaArgsCreate(&args));
		return enqueue(checkResult, async, result, func, args, 0, vargs...);
	}
};

//------------------------------------------------------------------------------
class StreamGuard {
		Stream&	m_stream;
	const	bool	m_sync;

public:
	StreamGuard(const StreamGuard&) = delete;

	inline StreamGuard(Stream& s, const bool sync) :
		m_stream(s),
		m_sync	(sync)
	{
		m_stream.lock();
	}

	inline ~StreamGuard(void) {
		if(m_sync)
			m_stream.sync();
		m_stream.unlock();
	}

	inline	Stream*		operator->	(void)		{	return &m_stream;	}
	inline	const Stream*	operator->	(void) const	{	return &m_stream;	}

	template<typename F, typename... Args>
	inline typename std::enable_if<is_function_ptr<F>::value, ReqId>::type enqueue(F func, const bool checkResult, ResultPtr result, const char* kernel, Args... args) {
		return m_stream.enqueue(func, checkResult, !m_sync, result, kernel, args...);
	}

	inline ReqId enqueue(const bool checkResult, ResultPtr result, const VEDAfunction& func, VEDAargs args, const int idx) {
		return m_stream.enqueue(checkResult, !m_sync, result, func, args, idx);
	}

	template<typename T, typename... Args>
	inline ReqId enqueue(const bool checkResult, ResultPtr result, const VEDAfunction& func, VEDAargs args, const int idx, const T value, Args... vargs) {
		return m_stream.enqueue(checkResult, !m_sync, result, func, args, idx, value, vargs...);
	}

	template<typename... Args>
	inline ReqId enqueue(const bool checkResult, ResultPtr result, const VEDAfunction& func, Args... vargs) {
		return m_stream.enqueue(checkResult, !m_sync, result, func, vargs...);
	}
};

//------------------------------------------------------------------------------
	}
}