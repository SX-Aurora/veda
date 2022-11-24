#pragma once

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
class Args final {
	VEDAargs	m_args;
	int		m_idx;

public:
	class Setter final {
		VEDAargs m_args;
		const int m_idx;

		inline Setter(VEDAargs args, const int idx) :
			m_args(args), m_idx(idx)
		{}

	public:
		void operator=(const Stack& stack);
		void operator=(const double value);
		void operator=(const float value);
		void operator=(const int16_t value);
		void operator=(const int32_t value);
		void operator=(const int64_t value);
		void operator=(const int8_t value);
		void operator=(const Base<VEDA>& ptr);
		void operator=(const Base<HMEM>& ptr);
		void operator=(const uint16_t value);
		void operator=(const uint32_t value);
		void operator=(const uint64_t value);
		void operator=(const uint8_t value);
		void operator=(const void* ptr);

		template<typename T>
		inline typename std::enable_if<
			std::is_class<T>::value &&
			!std::is_base_of<Base<VEDA>, T>::value &&
			!std::is_base_of<Base<HMEM>, T>::value
		>::type operator=(T& x) {
			operator=(Stack((void*)&x, VEDA_ARGS_INTENT_IN, sizeof(T)));
		}

		friend class Args;
	};

	 Args(void);
	~Args(void);

	inline Setter operator[](const int idx) {
		return {m_args, idx};
	}

	template<typename T>
	inline void append(T& x) {
		operator[](m_idx++) = x;
	}

	template<typename T, typename... A>
	inline void append(T& x, A&... args) {
		append(x);
		append(args...);
	}
	
	template<typename... T>
	inline Args(T&... args) : Args() {
		append(args...);
	}

	// TODO: make private?
	inline VEDAargs consume(void) {
		if(m_args == 0)
			VEDA_THROW(VEDA_ERROR_UNKNOWN); // already consumed!
		auto res = m_args;
		m_args = 0;
		return res;
	}
};

//------------------------------------------------------------------------------
	}
}