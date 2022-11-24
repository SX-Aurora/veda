#pragma once

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
template<typename T, size_t B>
void memset(T ptr, const void* value, const size_t cnt, const StreamId stream);

//------------------------------------------------------------------------------
template<typename B>
class Base {
	class Deleter {
		const StreamId m_stream;

	public:
		inline Deleter(const StreamId stream) : m_stream(stream) {}
		void operator()(const B ptr) const;
	};

protected:
	using Ptr = std::shared_ptr<typename std::remove_pointer<B>::type>;

		Ptr	m_ptr;
	const	size_t	m_cnt;
		size_t	m_offset;

	inline Base(Ptr ptr, const size_t cnt, const size_t offset) :
		m_ptr	(ptr),
		m_cnt	(cnt),
		m_offset(offset)
	{}

public:
	inline Base(B ptr, const size_t cnt, const StreamId stream = 0, const size_t offset = 0) :
		m_ptr	(ptr, Deleter(stream)),
		m_cnt	(cnt),
		m_offset(offset)
	{}

	inline B	raw	(void) const { return m_ptr.get() + m_offset;	}
	inline size_t	cnt	(void) const { return m_cnt;			}
	inline size_t	offset	(void) const { return m_offset;			}
	inline operator B	(void) const { return raw();			}
};

//------------------------------------------------------------------------------
class Copy {
	template<typename T>
	inline typename std::enable_if<std::is_pointer<T>::value, void*>::type deref(T x) {
		return (void*)x;
	}

	template<typename T> inline typename std::enable_if<std::is_base_of<Base<VEDA>, T>::value, VEDA>::type deref(T& x) { return (VEDA)x; }
	template<typename T> inline typename std::enable_if<std::is_base_of<Base<HMEM>, T>::value, HMEM>::type deref(T& x) { return (HMEM)x; }

	template<typename D, typename S>
	void copy_(D dst, S src, const size_t size, const StreamId stream);

protected:
	template<typename S, typename D>
	inline void copy(D dst, S src, const size_t size, const StreamId stream) {
		copy_(deref(dst), deref(src), size, stream);
	}
};

//------------------------------------------------------------------------------
template<typename B, typename T>
class Transfer : public Base<B>, public Copy {
protected:
	using Base<B>::Base;
	using Base<B>::m_ptr;
	using Base<B>::m_cnt;
	using Base<B>::m_offset;

	inline B get(void) const {
		return raw() + m_offset * sizeof(T);
	}

public:
	template<typename P>
	inline void to(P dst, const size_t cnt=0, const StreamId stream=0) {
		copy(dst, *this, sizeof(T) * cnt, stream);
	}

	template<typename P>
	inline void from(P src, const size_t cnt=0, const StreamId stream=0) {
		copy(*this, src, sizeof(T) * cnt, stream);
	}

	inline void memset(const T& value, const size_t cnt = 0, const StreamId stream = 0) const {
		cpp::memset<B, sizeof(T)>(get(), &value, cnt ? cnt : (m_cnt - m_offset), stream);
	}

	using Base<B>::raw;
	using Base<B>::cnt;
	using Base<B>::offset;
	using Base<B>::operator B;
};

//------------------------------------------------------------------------------
template<typename B, typename T>
struct Ref;

//------------------------------------------------------------------------------
template<typename B, typename T>
class Ptr final : public Transfer<B, T> {
	using Transfer<B, T>::m_ptr;
	using Transfer<B, T>::m_cnt;
	using Transfer<B, T>::m_offset;
	using Transfer<B, T>::get;

public:
	inline Ptr<B, T>& operator-=(const size_t cnt) {
		if(cnt > m_offset)
			VEDA_THROW(VEDA_ERROR_OUT_OF_BOUNDS);
		m_offset -= cnt;
		return *this;
	}

	inline Ptr<B, T>& operator+=(const size_t cnt) {
		if((cnt + m_offset) > m_cnt)
			VEDA_THROW(VEDA_ERROR_OUT_OF_BOUNDS);
		m_offset += cnt;
		return *this;
	}

		Ref<B, T>	operator[]	(const size_t cnt);
	inline	Ptr<B, T>	operator-	(const size_t cnt)	{ auto copy = *this; return copy -= cnt;	}
	inline	Ptr<B, T>	operator+	(const size_t cnt)	{ auto copy = *this; return copy += cnt;	}
	inline	Ptr<B, T>&	operator++	(void)			{ *this += 1; return *this;			}
	inline	Ptr<B, T>&	operator--	(void)			{ *this -= 1; return *this;			}

	template<typename C>
	inline Ptr<C, B> cast(void) {
		auto bytes	= m_cnt    * sizeof(T);
		auto offset	= m_offset * sizeof(T);
		if((bytes % sizeof(C)) || (offset % sizeof(C)))
			VEDA_THROW(VEDA_ERROR_MEMORY_MISALIGNED);
		return {get(), bytes/sizeof(C), offset/sizeof(C)};
	}

	using Transfer<B, T>::Transfer;
	using Transfer<B, T>::cnt;
	using Transfer<B, T>::from;
	using Transfer<B, T>::memset;
	using Transfer<B, T>::offset;
	using Transfer<B, T>::raw;
	using Transfer<B, T>::to;
	using Transfer<B, T>::operator B;
	friend class Device;
};

//------------------------------------------------------------------------------
template<typename B, typename T>
class Ref final : public Transfer<B, T> {
	using Transfer<B, T>::Transfer;

public:
	inline Ptr<B, T> operator&(void) {
		return *this;
	}

	using Transfer<B, T>::to;
	using Transfer<B, T>::from;
	using Transfer<B, T>::memset;
	using Transfer<B, T>::raw;
	using Transfer<B, T>::cnt;
	using Transfer<B, T>::offset;
	using Transfer<B, T>::operator B;
	friend class Ptr<B, T>;
};

//------------------------------------------------------------------------------
template<typename B, typename T>
inline Ref<B, T> Ptr<B, T>::operator[](const size_t c) {
	return {m_ptr, m_cnt, m_offset + c};
}

//------------------------------------------------------------------------------
	}
}

namespace veda {
	template<typename B, typename T>
	using Ptr = cpp::Ptr<B, T>;
}