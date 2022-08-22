#pragma once

namespace veda {
//------------------------------------------------------------------------------
struct Stream {
	using Calls = std::vector<std::tuple<uint64_t, bool, uint64_t*>>;
	veo_thr_ctxt*	ctx;
	Calls		calls;

	inline Stream(void)	: ctx(0) {}
	inline Stream(Stream&&)	: ctx(0) {}
};

//------------------------------------------------------------------------------
class StreamGuard {
	Stream&			m_stream;
	std::recursive_mutex	m_mutex;

public:
				StreamGuard	(Stream& s);
				~StreamGuard	(void);
	const Stream::Calls&	calls		(void) const;
	int			state		(void) const;
	uint64_t		wait		(const int id) const;
	void			clear		(void);
	void			enqueue		(const bool checkResult, uint64_t* result, VEDAfunction func, VEDAargs args, const int idx);
	void			enqueue		(const uint64_t req, const bool checkResult, uint64_t* result);

	template<typename F, typename... Args>
	inline void enqueue(F func, const bool checkResult, uint64_t* result, Args... args) {
		uint64_t req = CREQ(func(m_stream.ctx, args...));
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
}