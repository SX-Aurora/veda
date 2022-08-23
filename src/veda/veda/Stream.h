#pragma once

namespace veda {
//------------------------------------------------------------------------------
struct Stream {
	using Calls = std::vector<std::tuple<uint64_t, bool, uint64_t*>>;

private:
	veo_thr_ctxt*	m_ctx;
	Calls		m_calls;

	uint64_t	wait	(const uint64_t id) const;

public:
	Stream(veo_thr_ctxt* ctx);

	const Calls&	calls	(void) const;
	int		state	(void) const;
	void		enqueue	(const bool checkResult, uint64_t* result, VEDAfunction func, VEDAargs args, const int idx);
	void		enqueue	(const uint64_t req, const bool checkResult, uint64_t* result);
	void		lock	(void);
	void		sync	(void);
	void		unlock	(void);

	template<typename F, typename... Args>
	inline void enqueue(F func, const bool checkResult, uint64_t* result, Args... args) {
		uint64_t req = CREQ(func(m_ctx, args...));
		enqueue(req, checkResult, result);
	}

	template<typename T, typename... Args>
	inline void enqueue(const bool checkResult, uint64_t* result, VEDAfunction func, VEDAargs args, const int idx, const T value, Args... vargs) {
		TVEDA(vedaArgsSet(args, idx, value));
		enqueue(checkResult, result, func, args, idx+1, vargs...);
	}

	template<typename... Args>
	inline void enqueue(const bool checkResult, uint64_t* result, VEDAfunction func, Args... vargs) {
		VEDAargs args = 0;
		TVEDA(vedaArgsCreate(&args));
		enqueue(checkResult, result, func, args, 0, vargs...);
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