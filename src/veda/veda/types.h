#pragma once

#include <stdint.h>
#include "enums.h"
#include "macros.h"

struct veo_args;
typedef struct veo_args*	VEDAargs;
typedef uint64_t		veo_ptr;

typedef int			VEDAdevice;
typedef uint32_t		VEDAidx;
typedef uint64_t		VEDAoffset;
typedef int			VEDAstream;
typedef uint64_t		VEDAfunction;
typedef uint64_t (*VEDAhost_function)(void*);
typedef void (*VEDAstream_callback)(VEDAstream, VEDAresult, void*);

typedef struct {
	int8_t _;
} VEDAdeviceptr_;

typedef VEDAdeviceptr_* VEDAdeviceptr;

#if __cplusplus
	template<typename T = char>
	class VEDAptr {
		VEDAdeviceptr m_ptr;

	public:
		struct Tuple {
			T* const	ptr;
			const size_t	size;
			inline Tuple(T* _ptr, const size_t _size) : ptr(_ptr), size(_size) {}
		};

		class Ref {
			const VEDAdeviceptr m_ptr;

		public:
			inline 			Ref		(const VEDAdeviceptr ptr) : m_ptr(ptr)	{			}
			inline VEDAptr<T>	operator&	(void) const				{ return {m_ptr};	}
		};

		class Ptr {
			VEDAptr<T>& m_ref;
		public:
			inline		Ptr			(VEDAptr<T>& ref) : m_ref(ref)	{			}
			inline operator const VEDAdeviceptr*	(void) const			{ return &m_ref.m_ptr;	}
			inline operator VEDAdeviceptr*		(void)				{ return &m_ref.m_ptr;	}
		};

		inline			VEDAptr			(const VEDAdeviceptr ptr = 0)	: m_ptr(ptr)	{									}
		inline			VEDAptr			(const VEDAptr<T>& o)		: m_ptr(o.m_ptr){									}
		inline			operator VEDAdeviceptr	(void) const					{	return m_ptr;							}
		inline	Ptr		operator&		(void) 						{	return {*this};							}
		inline	Ref		operator*		(void) const					{	return {m_ptr};							}
		inline	Ref		operator[]		(const size_t offset) const			{	return m_ptr + offset * sizeof(T);				}
		inline	VEDAdevice	device			(void) const					{	return VEDA_GET_DEVICE(m_ptr);					}
		inline	VEDAidx		idx			(void) const					{	return VEDA_GET_IDX(m_ptr);					}
		inline	VEDAptr<T>	base			(void) const					{	return (VEDAdeviceptr)((uint64_t)m_ptr & ~VEDA_BITS_OFFSET);	}
		inline	VEDAptr<T>	operator+		(const size_t offset) const			{	return m_ptr + offset * sizeof(T);				}
		inline	VEDAptr<T>	operator++		(int)						{	auto o = m_ptr; m_ptr += sizeof(T); return o;			}
		inline	VEDAptr<T>	operator--		(int)						{	auto o = m_ptr; m_ptr -= sizeof(T); return o;			}
		inline	VEDAptr<T>&	operator++		(void)						{	return *this += 1;						}
		inline	VEDAptr<T>&	operator+=		(const size_t offset)				{	m_ptr += offset * sizeof(T); return *this;			}
		inline	VEDAptr<T>&	operator--		(void)						{	return *this -= 1;						}
		inline	VEDAptr<T>&	operator-=		(const size_t offset)				{	m_ptr -= offset * sizeof(T); return *this;			}
		inline	size_t		offset			(void) const					{	return VEDA_GET_OFFSET(m_ptr);					}

			Tuple		ptrSize			(void) const;
			size_t		size			(void) const;
			T*		ptr			(void) const;

		template<typename TT>
		inline operator VEDAptr<TT> (void) const {
			return m_ptr;
		}

	#ifndef __NEC__
				operator void*		(void) const;
		void*		hmem			(void) const;
	#endif
	};
#endif

#ifdef __cplusplus
	namespace veda {
		class Module;
		class Context;
	}

	typedef veda::Context*		VEDAcontext;
	typedef veda::Module*		VEDAmodule;
#else
	struct __VEDAcontext;
	struct __VEDAmodule;
	typedef struct __VEDAcontext*	VEDAcontext;
	typedef struct __VEDAmodule*	VEDAmodule;
#endif
