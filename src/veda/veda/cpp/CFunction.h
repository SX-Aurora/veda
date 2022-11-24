#pragma once

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
class CFunctionBase : public VEDAfunction {
	Context m_ctx; // don't make const because of Template<...>::...::Args<...>()

public:
	class Launcher {
		const CFunctionBase&	m_func;
		const StreamId		m_stream;

		Future::ReqId launch(Args& args, Future::Ptr ptr = {});

	protected:
		inline Launcher(const CFunctionBase& func, const StreamId stream) :
			m_func(func),
			m_stream(stream)
		{}
		
		template<typename R>
		inline RType<R> launch(Args& args) {
			if constexpr (std::is_same<void, R>::value) {
				launch(args);
			} else {
				TypedFuture<R> res(m_func.m_ctx, m_stream);
				res.set(launch(args, res.ptr()));
				return res;
			}
		}

		friend class CFunctionBase;
	};

	CFunctionBase(void);
	CFunctionBase(Module mod, const std::string_view name);
};

//------------------------------------------------------------------------------
template<typename R>
class CFunction : public CFunctionBase {
	static_assert(std::is_fundamental<R>::value || std::is_pointer<R>::value, "Only fundamental and pointer types allowed as return value");

public:
	struct Launcher : public CFunctionBase::Launcher {
		template<typename... A>
		inline RType<R> operator()(A... vargs) {
			Args args(vargs...);
			return launch<R>(args);
		}

		using CFunctionBase::Launcher::Launcher;
		friend class CFunction;
	};

	inline Launcher operator[](const StreamId stream) const {
		return {*this, stream};
	}

	template<typename... A>
	inline RType<R> operator()(A... vargs) const {
		return operator[](0)(vargs...);
	}

	using CFunctionBase::CFunctionBase;
};

//------------------------------------------------------------------------------
	}
}

namespace veda {
	struct CFunction : public cpp::CFunction<void> {
		template<typename T>
		struct Return : public cpp::CFunction<T> {
			using cpp::CFunction<T>::CFunction;
		};
		using cpp::CFunction<void>::CFunction;
	};
}