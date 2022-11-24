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
typedef uint64_t (*VEDAhost_function)(void*);
typedef void (*VEDAstream_callback)(VEDAstream, VEDAresult, void*);

typedef struct VEDAfunction_struct {
	uint64_t	ptr;
	const char*	kernel;

#if __cplusplus
	inline VEDAfunction_struct(void) : ptr(0), kernel(0) {}
	inline operator bool(void) const {
		return ptr != 0;
	}
#endif
} VEDAfunction;

#define VEDA_PROFILER_STRUCT				\
/* 32-bit */	const VEDAprofiler_type	type;		\
		const int		device_id;	\
		const int		stream_id;	\
/* 64-bit */	uint64_t		req_id;		\
		void* 			user_data;
// put 64bit vars last, so we ensure alignment with Data<...>

typedef struct VEDAprofiler_data_struct {
	VEDA_PROFILER_STRUCT

#if __cplusplus
	inline VEDAprofiler_data_struct(const VEDAprofiler_type _type, const int _device_id, const int _stream_id) :
		req_id		(-1),
		user_data	(0),
		type		(_type),
		device_id	(_device_id),
		stream_id	(_stream_id)
	{}
#endif
} VEDAprofiler_data;

typedef struct	{	VEDA_PROFILER_STRUCT	void* const dst; void* const src; const size_t bytes;	}	VEDAprofiler_vedaMemcpy;
typedef struct	{	VEDA_PROFILER_STRUCT	void* const func; const char* const kernel;		}	VEDAprofiler_vedaLaunchKernel;
typedef struct	{	VEDA_PROFILER_STRUCT	void* const func; 					}	VEDAprofiler_vedaLaunchHostFunc;
typedef struct	{	VEDA_PROFILER_STRUCT	const size_t bytes;					}	VEDAprofiler_vedaMemAlloc;
typedef struct	{	VEDA_PROFILER_STRUCT	void* const ptr;					}	VEDAprofiler_vedaMemFree;

typedef void (*VEDAprofiler_callback)(VEDAprofiler_data* data, const int enter);

typedef struct { int8_t _; } VEDAdeviceptr_;
typedef struct { int8_t _; } VEDAhmemptr_;

typedef VEDAdeviceptr_*	VEDAdeviceptr;
typedef VEDAhmemptr_*	VEDAhmemptr;

#if __cplusplus
	template<typename T = char>
	class VEDAptr final {
		VEDAdeviceptr m_ptr;

	public:
		struct Tuple {
			T* const	ptr;
			const size_t	size;
			inline Tuple(T* _ptr, const size_t _size) : ptr(_ptr), size(_size) {}
		};

		class Ref final {
			const VEDAdeviceptr m_ptr;

		public:
			inline 			Ref		(const VEDAdeviceptr ptr) : m_ptr(ptr)	{			}
			inline VEDAptr<T>	operator&	(void) const				{ return {m_ptr};	}
		};

		class Ptr final {
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
	};

	template<typename T = char>
	class HMEMptr final {
		VEDAhmemptr m_ptr;

	public:
		class Ref final {
			const VEDAhmemptr m_ptr;

		public:
			inline 			Ref		(const VEDAhmemptr ptr) : m_ptr(ptr)	{			}
			inline HMEMptr<T>	operator&	(void) const				{ return {m_ptr};	}
		};

		class Ptr final {
			HMEMptr<T>& m_ref;
		public:
			inline		Ptr			(HMEMptr<T>& ref) : m_ref(ref)	{			}
			inline operator const VEDAhmemptr*	(void) const			{ return &m_ref.m_ptr;	}
			inline operator VEDAhmemptr*		(void)				{ return &m_ref.m_ptr;	}
		};

		inline			HMEMptr			(const VEDAhmemptr ptr = 0)	: m_ptr(ptr)	{									}
		inline			HMEMptr			(const HMEMptr<T>& o)		: m_ptr(o.m_ptr){									}
		inline			operator VEDAhmemptr	(void) const					{	return m_ptr;							}
		inline	Ptr		operator&		(void) 						{	return {*this};							}
		inline	Ref		operator*		(void) const					{	return {m_ptr};							}
		inline	Ref		operator[]		(const size_t offset) const			{	return m_ptr + offset * sizeof(T);				}
		inline	HMEMptr<T>	operator+		(const size_t offset) const			{	return m_ptr + offset * sizeof(T);				}
		inline	HMEMptr<T>	operator++		(int)						{	auto o = m_ptr; m_ptr += sizeof(T); return o;			}
		inline	HMEMptr<T>	operator--		(int)						{	auto o = m_ptr; m_ptr -= sizeof(T); return o;			}
		inline	HMEMptr<T>&	operator++		(void)						{	return *this += 1;						}
		inline	HMEMptr<T>&	operator+=		(const size_t offset)				{	m_ptr += offset * sizeof(T); return *this;			}
		inline	HMEMptr<T>&	operator--		(void)						{	return *this -= 1;						}
		inline	HMEMptr<T>&	operator-=		(const size_t offset)				{	m_ptr -= offset * sizeof(T); return *this;			}
			T*		ptr			(void) const;

		template<typename TT>
		inline operator HMEMptr<TT> (void) const {
			return m_ptr;
		}
	};
#endif

#ifdef __cplusplus
	namespace veda {
		namespace internal {
			class Module;
			class Context;
		}
	}

	using VEDAcontext = veda::internal::Context*;
	using VEDAmodule  = veda::internal::Module*;
#else
	struct __VEDAcontext;
	struct __VEDAmodule;
	typedef struct __VEDAcontext*	VEDAcontext;
	typedef struct __VEDAmodule*	VEDAmodule;
#endif
