#pragma once

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
struct Future {
	using Ptr	= std::shared_ptr<uint64_t>;
	using ReqId	= uint64_t;

private:
	Context		m_ctx;
	const StreamId	m_stream;
	Ptr		m_result;
	ReqId		m_until;

public:
	// TODO: make private
	inline Future(internal::Context* ctx, const StreamId stream) :
		m_ctx	(ctx),
		m_stream(stream),
		m_result(new uint64_t),
		m_until	(-1)
	{}

	inline Future(const Future& o) :
		m_ctx	(o.m_ctx),
		m_stream(o.m_stream),
		m_result(o.m_result),
		m_until (o.m_until)
	{}
	
		void	wait	(void);
	inline	Ptr	ptr	(void)			{ return m_result;	}
	inline	void	set	(const ReqId id)	{ m_until = id;		}

	template<typename T = uint64_t>
	inline typename std::enable_if<std::is_fundamental<T>::value, T>::type get(void) {
		wait();
		return ((const T*)m_result.get())[sizeof(uint64_t) / sizeof(T) - 1];
	}

	friend class Device;
};

//------------------------------------------------------------------------------
template<typename T>
struct TypedFuture final : public Future {
	inline T get		(void) { return Future::get<T>();	}
	inline operator T	(void) { return get();			}
	using Future::Future;
};

//------------------------------------------------------------------------------
template<typename R>
using RType = typename std::conditional<std::is_same<R, void>::value, void, TypedFuture<R>>::type;

//------------------------------------------------------------------------------
	}
}