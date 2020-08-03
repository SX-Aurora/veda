#include "veda_internal.h"
#include <cstdio>

#define LOCK() std::lock_guard<std::mutex> lock(m_mutex)

//------------------------------------------------------------------------------
static VEDAresult vedaCtxCall(VEDAcontext ctx, bool checkResults, VEDAfunction func, VEDAargs args, const int idx) {
	return ctx->call(func, args, true, checkResults);
}

//------------------------------------------------------------------------------
template<typename T, typename... Args>
static VEDAresult vedaCtxCall(VEDAcontext ctx, bool checkResults, VEDAfunction func, VEDAargs args, const int idx, const T value, Args... vargs) {
	CVEDA(vedaArgsSet(args, idx, value));
	return vedaCtxCall(ctx, checkResults, func, args, idx+1, vargs...);
}

//------------------------------------------------------------------------------
template<typename... Args>
static VEDAresult vedaCtxCall(VEDAcontext ctx, bool checkResults, VEDAfunction func, Args... vargs) {
	VEDAargs args = 0;
	CVEDA(vedaArgsCreate(&args));
	return vedaCtxCall(ctx, checkResults, func, args, 0, vargs...);
}

//------------------------------------------------------------------------------
__VEDAcontext::__VEDAcontext(VEDAproc proc, veo_thr_ctxt* ctx) : m_proc(proc), m_ctx(ctx) {
	assert(proc); assert(ctx);
	m_calls.reserve(128);
}

//------------------------------------------------------------------------------
__VEDAcontext::~__VEDAcontext(void) {
}

//------------------------------------------------------------------------------
VEDAproc		__VEDAcontext::proc	(void) const {	return m_proc;	}
veo_thr_ctxt*	__VEDAcontext::ctx	(void) const {	return m_ctx;	}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memcpyD2H(void* dst, VEDAdeviceptr src, const size_t bytes) {
	size_t size; veo_ptr ptr;
	CVEDA(getPtr(&ptr, &size, src));
	TRACE("VEDAcontext::memcpyD2H(%p, %p, %llu), ptr=%p, size=%llu\n", dst, src, bytes, ptr, size);
	LOCK();
	m_calls.emplace_back(veo_async_read_mem(ctx(), dst, ptr, bytes), (VEDAargs)0, false);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::getPtr(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr) {
	// does the host already know this ptr?
	CVEDA(proc()->getBasePtr(ptr, size, vptr));
	if(*ptr == 0)
		return VEDA_ERROR_UNKNOWN_PPTR;

	VEDAptr pptr(vptr);
	*ptr = *ptr + pptr.offset();

	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memcpyH2D(VEDAdeviceptr dst, const void* src, const size_t bytes) {
	size_t size; veo_ptr ptr;
	CVEDA(getPtr(&ptr, &size, dst));
	TRACE("VEDAcontext::memcpyH2D(%p, %p, %llu), ptr=%p, size=%llu\n", dst, src, bytes, ptr, size);
	LOCK();
	m_calls.emplace_back(veo_async_write_mem(ctx(), ptr, src, bytes), (VEDAargs)0, false);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::call(VEDAfunction func, VEDAargs args, const bool destroyArgs, const bool checkResult) {
	LOCK();
	TRACE("VEDAcontext::call(%p (%s), %p, %i, %i)\n", func, proc()->kernelName(func), args, destroyArgs, checkResult);
	m_calls.emplace_back(veo_call_async(ctx(), func, args), destroyArgs ? args : 0, checkResult);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::call(VEDAhost_function func, void* userData) {
	LOCK();
	TRACE("VEDAcontext::call(%p, %p)\n", func, userData);
	m_calls.emplace_back(veo_call_async_vh(ctx(), func, userData), (VEDAargs)0, false);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memAlloc(VEDAdeviceptr* vptr, const size_t size) {
	veo_ptr* ptr;
	*vptr = proc()->newVPTR(&ptr, size);
	TRACE("VEDAcontext::memAlloc(%p, %llu)\n", *vptr, size);
	if(size)
		CVEDA(vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEM_ALLOC), VEDAstack(ptr, VEDA_ARGS_INTENT_INOUT, sizeof(veo_ptr)), *vptr, size));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memAllocPitch(VEDAdeviceptr* vptr, size_t* pitch, const size_t w_bytes, const size_t h, const uint32_t elementSize) {
	*pitch = w_bytes;
	return memAlloc(vptr, w_bytes * h);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memFree(VEDAdeviceptr vptr) {
	if(VEDAptr(vptr).offset() != 0)
		return VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED;

	size_t size; veo_ptr ptr;

	// get physical pointer
	auto err = getPtr(&ptr, &size, vptr);
	TRACE("VEDAcontext::memFree(%p), ptr=%p, size=%llu\n", vptr, ptr, size);
	// if(err == VEDA_ERROR_UNKNOWN_PPTR):	has already been freed on device
	// if(err == VEDA_SUCCESS):				needs to be freed from host

	// free vptr on device
	if(err == VEDA_SUCCESS) {
		CVEDA(vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEM_FREE), vptr));
	}

	// free vptr on host
	if(err == VEDA_ERROR_UNKNOWN_PPTR || err == VEDA_SUCCESS) {
		CVEDA(proc()->free(vptr));
		return VEDA_SUCCESS;
	}

	return err;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memPtr(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr) {
	TRACE("VEDAcontext::memPtr(%p, %llu, %p)\n", *ptr, *size, vptr);
	return vedaCtxCall(this, false, proc()->kernel(VEDA_KERNEL_MEM_PTR), 
		VEDAstack(ptr, VEDA_ARGS_INTENT_INOUT, sizeof(veo_ptr)),
		VEDAstack(size, VEDA_ARGS_INTENT_INOUT, sizeof(size_t)), 
		vptr);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memcpyD2D(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size) {
	TRACE("VEDAcontext::memcpyD2D(%p, %p, %llu)\n", dst, src, size);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMCPY_D2D), dst, src, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset(VEDAdeviceptr dst, const uint8_t value, const size_t size) {
	TRACE("VEDAcontext::memsetU8(%p, %u, %llu)\n", dst, value, size);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMSET_U8), dst, value, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset(VEDAdeviceptr dst, const uint16_t value, const size_t size) {
	TRACE("VEDAcontext::memsetU16(%p, %u, %llu)\n", dst, value, size);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMSET_U16), dst, value, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset(VEDAdeviceptr dst, const uint32_t value, const size_t size) {
	TRACE("VEDAcontext::memsetU32(%p, %u, %llu)\n", dst, value, size);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMSET_U32), dst, value, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset(VEDAdeviceptr dst, const uint64_t value, const size_t size) {
	TRACE("VEDAcontext::memsetU64(%p, %llu, %llu)\n", dst, value, size);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMSET_U64), dst, value, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h) {
	TRACE("VEDAcontext::memsetU8_2D(%p, %lluu, %u, %llu)\n", dst, value, w, h);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMSET_U8_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h) {
	TRACE("VEDAcontext::memsetU16_2D(%p, %lluu, %u, %llu)\n", dst, value, w, h);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMSET_U16_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h) {
	TRACE("VEDAcontext::memsetU32_2D(%p, %lluu, %u, %llu)\n", dst, value, w, h);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMSET_U32_2D), pitch, value, w, h);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h) {
	TRACE("VEDAcontext::memsetU64_2D(%p, %lluu, %llu, %llu)\n", dst, value, w, h);
	return vedaCtxCall(this, true, proc()->kernel(VEDA_KERNEL_MEMSET_U64_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::query(void) {
	LOCK();
	auto state = veo_get_context_state(ctx());
	switch(state) {
		case VEO_STATE_UNKNOWN:	return VEDA_ERROR_VEO_STATE_UNKNOWN;
		case VEO_STATE_RUNNING:	return VEDA_ERROR_VEO_STATE_RUNNING;
		case VEO_STATE_SYSCALL:	return VEDA_ERROR_VEO_STATE_SYSCALL;
		case VEO_STATE_BLOCKED:	return VEDA_ERROR_VEO_STATE_BLOCKED;
		case VEO_STATE_EXIT:	return VEDA_SUCCESS; // hope this is correct
	}
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::sync(void) {
	TRACE("VEDAcontext::sync(), calls=%llu\n", m_calls.size());
	LOCK();
	auto err = [&](void) {
		for(auto& it : m_calls) {
			auto id = std::get<0>(it);
			auto args = std::get<1>(it);
			auto checkResult = std::get<2>(it);

			uint64_t res = 0;
			CVEO(veo_call_wait_result(ctx(), id, &res));
			VEDAresult _res = (VEDAresult)res;

			if(checkResult)
				CVEDA(_res);

			if(args)
				CVEDA(vedaArgsDestroy(args));
		}
		return VEDA_SUCCESS;
	}();
	m_calls.clear();
	#ifndef NDEBUG
	if(err != VEDA_SUCCESS) {
		const char* str;
		vedaGetErrorName(err, &str);
		TRACE("ERROR: %s\n", str);
		exit(1);
	}
	#endif
	return err;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::streamCreate(VEDAstream* stream) {
	LOCK();
	m_streams.emplace_back(this);
	*stream = &m_streams.back();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::streamDestroy(VEDAstream stream) {
	LOCK();
	for(auto it = m_streams.begin(); it != m_streams.end(); it++) {
		if(&*it == stream) {
			m_streams.erase(it);
			return VEDA_SUCCESS;
		}
	}
	return VEDA_ERROR_UNKNOWN_STREAM;
}

//------------------------------------------------------------------------------