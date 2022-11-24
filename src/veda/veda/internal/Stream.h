#pragma once

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
struct Stream {
	using Calls	= std::deque<std::tuple<ReqId, bool, ResultPtr>>;

private:
	const int	m_deviceId;
	const int	m_streamId;
	veo_thr_ctxt*	m_ctx;
	Calls		m_calls;

	uint64_t	wait		(const uint64_t id) const;
	ReqId		enqueueProfiler	(void* data, const bool enter);

	inline ReqId enqueue(const uint64_t req, const bool checkResult, ResultPtr result) {
		m_calls.emplace_back(req, checkResult, result);
		return req;
	}

public:
		Stream	(veo_thr_ctxt* ctx, const int deviceId, const int streamId);
	void	sync	(const ReqId until = -1);

	inline const Calls&	calls	(void) const	{ return m_calls;			}
	inline int		state	(void) const	{ return veo_get_context_state(m_ctx);	}
	inline void		lock	(void)		{ veo_req_block_begin(m_ctx);		}
	inline void		unlock	(void)		{ veo_req_block_end(m_ctx);		}

	template<typename F, typename... Args>
	inline typename std::enable_if<is_function_ptr<F>::value, ReqId>::type enqueue(F func, const bool checkResult, ResultPtr result, const char* kernel, Args... args) {
		if(auto data = profiler::data(m_deviceId, m_streamId, func, args..., kernel)) {
			// in case the user didn't provide a result, we use the one of data
			enqueueProfiler	(data, true);
			data->req_id = CREQ(func(m_ctx, args...));
			enqueue		(data->req_id, checkResult, result);
			return enqueueProfiler(data, false);
		} else {
			return enqueue(CREQ(func(m_ctx, args...)), checkResult, result);
		}
	}

	inline ReqId enqueue(const bool checkResult, ResultPtr result, const VEDAfunction& func, VEDAargs args, const int idx) {
		auto id = enqueue(veo_call_async, checkResult, result, func.kernel, func.ptr, args);
		TVEDA(vedaArgsDestroy(args));
		return id;
	}

	template<typename T, typename... Args>
	inline ReqId enqueue(const bool checkResult, ResultPtr result, const VEDAfunction& func, VEDAargs args, const int idx, const T value, Args... vargs) {
		TVEDA(vedaArgsSet(args, idx, value));
		return enqueue(checkResult, result, func, args, idx+1, vargs...);
	}

	template<typename... Args>
	inline ReqId enqueue(const bool checkResult, ResultPtr result, const VEDAfunction& func, Args... vargs) {
		VEDAargs args = 0;
		TVEDA(vedaArgsCreate(&args));
		return enqueue(checkResult, result, func, args, 0, vargs...);
	}
};

//------------------------------------------------------------------------------
class StreamGuard {
	Stream&	m_stream;

public:
	inline			StreamGuard	(Stream& s) : m_stream(s)	{	m_stream.lock();	}
	inline			~StreamGuard	(void)				{	m_stream.unlock();	}
	inline	Stream*		operator->	(void)				{	return &m_stream;	}
	inline	const Stream*	operator->	(void) const			{	return &m_stream;	}
};

//------------------------------------------------------------------------------
	}
}