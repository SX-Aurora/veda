#pragma once

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
class HostFunctionBase {
protected:
	const veda::Context m_ctx;

	Future::ReqId launch(const StreamId stream, VEDAhost_function func, void* data, Future::Ptr ptr) const;

public:
		HostFunctionBase(const Device& dev);

	template<typename R>
	inline RType<R> launch(const StreamId stream, VEDAhost_function func, void* data) const {
		if constexpr(std::is_same<void, R>::value) {
			launch(stream, func, data, {});
		} else {
			TypedFuture<R> res(m_ctx, stream);
			res.set(launch(stream, func, data, res.ptr()));
			return res;
		}
	}
};
		
//------------------------------------------------------------------------------
template<typename R, typename... A>
struct HostFunction final : public HostFunctionBase {
	static_assert(std::is_fundamental<R>::value, "Only fundamental return types are allowed");

	using Func	= uint64_t(*)(A...);
	using Tuple	= typename std::tuple<A...>;

	struct Data {
		Func	func;
		Tuple	tuple;

		inline Data(Func func_, Tuple&& tuple_) :
			func	(func_),
			tuple	(tuple_)
		{}
	};

	// stolen from: https://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
	template<int ...>
	struct seq {};

	template<int N, int ...S>
	struct gens : gens<N-1, N-1, S...> { };

	template<int ...S>
	struct gens<0, S...> {
  		typedef seq<S...> type;
	};
	// ...

	template<int... S>
	static inline uint64_t launch(Data* data, seq<S...>) {
		return data->func(std::get<S>(data->tuple)...);
	}

	static inline uint64_t callback(void* data_) {
		auto data	= (Data*)data_;
		auto res	= launch(data, typename gens<sizeof...(A)>::type());
		delete data;
		return res;
	};

private:
	Func m_func;

public:
	class Launcher {
		const HostFunction<R, A...>&	m_func;
		const StreamId			m_stream;

	protected:
		inline Launcher(const HostFunction<R, A...>& func, const StreamId stream) :
			m_func(func),
			m_stream(stream)
		{}

	public:
		inline RType<R> operator()(A... args) const {
			auto data = new Data(m_func.m_func, std::make_tuple(args...));
			return m_func.HostFunctionBase::launch<R>(m_stream, &HostFunction<R, A...>::callback, data);
		}

		friend class HostFunction<R, A...>;
	};

	inline Launcher operator[](const StreamId stream) const {
		return {*this, stream};
	}

	inline RType<R> operator()(A... args) const {
		return operator[](0)(args...);
	}

	inline HostFunction(const Device& dev, Func func) :
		HostFunctionBase(dev),
		m_func		(func)
	{}
};

//------------------------------------------------------------------------------
	}
}

namespace veda {
	template<typename R, typename... A>
	inline cpp::HostFunction<R, A...> HostFunction(const cpp::Device& dev, R (*func)(A...)) {
		return {dev, func};
	}
}