#include "veda_internal.h"

#define LOCK() std::lock_guard<std::mutex> lock(m_mutex)

//------------------------------------------------------------------------------
static VEDAresult vedaCtxCall(VEDAcontext ctx, VEDAstream stream, bool checkResults, VEDAfunction func, VEDAargs args, const int idx) {
	return ctx->call(func, stream, args, true, checkResults);
}

//------------------------------------------------------------------------------
template<typename T, typename... Args>
static VEDAresult vedaCtxCall(VEDAcontext ctx, VEDAstream stream, bool checkResults, VEDAfunction func, VEDAargs args, const int idx, const T value, Args... vargs) {
	CVEDA(vedaArgsSet(args, idx, value));
	return vedaCtxCall(ctx, stream, checkResults, func, args, idx+1, vargs...);
}

//------------------------------------------------------------------------------
template<typename... Args>
static VEDAresult vedaCtxCall(VEDAcontext ctx, VEDAstream stream, bool checkResults, VEDAfunction func, Args... vargs) {
	VEDAargs args = 0;
	CVEDA(vedaArgsCreate(&args));
	return vedaCtxCall(ctx, stream, checkResults, func, args, 0, vargs...);
}

//------------------------------------------------------------------------------
VEDAdevice __VEDAcontext::device(void) const {
	return m_device;
}

//------------------------------------------------------------------------------
__VEDAcontext::__VEDAcontext(VEDAdevice device, veo_proc_handle* handle, const int numStreams) :
	m_device	(device),
	m_handle	(handle),
	m_lib		(0),
	m_memidx	(1)
{
	TRACE("VEDAcontext::VEDAcontext(%i, %p)\n", device, handle);
	m_kernels.resize(VEDA_KERNEL_CNT);
	m_streams.resize(numStreams);
}

//------------------------------------------------------------------------------
__VEDAcontext::~__VEDAcontext(void) {
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memReport(void) {
	CVEDA(syncPtrs());
	size_t total;
	CVEDA(vedaDeviceTotalMem(&total, device()));
	size_t used = 0;
	for(auto& it : m_ptrs)
		used += std::get<1>(it.second);

	printf("# VE#%i %.2f/%.2fGB\n", device(), used/(1024.0*1024.0*1024.0), total/(1024.0*1024.0*1024.0));
	for(auto& it : m_ptrs) {
		VEDAptr vptr(device(), it.first);
		auto ptr	= std::get<0>(it.second);
		auto size	= std::get<1>(it.second);
		printf("%p/%p %lluB\n", vptr.vptr(), ptr, size);
	}
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::destroy(void) {
	TRACE("VEDAcontext::~VEDAcontext(%i)\n", device());
	CVEDA(syncPtrs());
	for(auto& it : m_ptrs) {
		auto idx	= it.first;
		auto& size	= std::get<1>(it.second);

		VEDAptr vptr(device(), idx);
		printf("[VEDA ERROR]: VEDAdeviceptr %p with size %lluB has not been freed!\n", vptr.vptr(), size);
	}

	CVEO(veo_proc_destroy(m_handle));
	m_streams.clear();	// don't need to be destroyed
	m_modules.clear();	// don't need to be destroyed
	m_kernels.clear();	// don't need to be destroyed
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::init(void) {
	TRACE("VEDAcontext::init(%i)\n", device());
	assert(m_lib == 0);

	CVEDA(moduleLoad(&m_lib, vedaModuleStdLib()));
	for(int i = 0; i < VEDA_KERNEL_CNT; i++) {
		const char* name = 0;
		CVEDA(vedaKernelGetName(&name, (VEDAkernel)i));
		CVEDA(moduleGetFunction(&m_kernels[i], m_lib, name));
	}

	for(auto& stream : m_streams) {
		assert(stream.ctx == 0);
		stream.ctx = veo_context_open(m_handle);
		if(stream.ctx == 0)
			return VEDA_ERROR_CANNOT_CREATE_STREAM;
		stream.calls.reserve(128);
		assert(ref.calls.empty());
	}
	
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
size_t __VEDAcontext::streamCount(void) const {
	return m_streams.size();
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::stream(veo_thr_ctxt** veo, const VEDAstream stream) {
	if(stream < 0 || stream >= m_streams.size() || m_streams[stream].ctx == 0)
		return VEDA_ERROR_UNKNOWN_STREAM;
	*veo = m_streams[stream].ctx;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
// Kernels
//------------------------------------------------------------------------------
const char* __VEDAcontext::kernelName(VEDAfunction func) const {
	int idx = 0;
	for(; idx < VEDA_KERNEL_CNT; idx++) {
		if(m_kernels[idx] == func)
			break;
	}

	switch(idx) {
		case VEDA_KERNEL_MEMSET_U8:		return "VEDA_KERNEL_MEMSET_U8";
		case VEDA_KERNEL_MEMSET_U16:	return "VEDA_KERNEL_MEMSET_U16";
		case VEDA_KERNEL_MEMSET_U32:	return "VEDA_KERNEL_MEMSET_U32";
		case VEDA_KERNEL_MEMSET_U64:	return "VEDA_KERNEL_MEMSET_U64";
		case VEDA_KERNEL_MEMSET_U8_2D:	return "VEDA_KERNEL_MEMSET_U8_2D";
		case VEDA_KERNEL_MEMSET_U16_2D:	return "VEDA_KERNEL_MEMSET_U16_2D";
		case VEDA_KERNEL_MEMSET_U32_2D:	return "VEDA_KERNEL_MEMSET_U32_2D";
		case VEDA_KERNEL_MEMSET_U64_2D:	return "VEDA_KERNEL_MEMSET_U64_2D";
		case VEDA_KERNEL_MEMCPY_D2D:	return "VEDA_KERNEL_MEMCPY_D2D";
		case VEDA_KERNEL_MEM_ALLOC:		return "VEDA_KERNEL_MEM_ALLOC";
		case VEDA_KERNEL_MEM_FREE:		return "VEDA_KERNEL_MEM_FREE";
		case VEDA_KERNEL_MEM_PTR:		return "VEDA_KERNEL_MEM_PTR";
	}

	return "USER_KERNEL";
}

//------------------------------------------------------------------------------
VEDAfunction __VEDAcontext::kernel(VEDAkernel kernel) const {
	if(kernel < 0 || kernel >= VEDA_KERNEL_CNT)
		return VEDA_ERROR_UNKNOWN_KERNEL;
	return m_kernels[kernel];
}

//------------------------------------------------------------------------------
// Modules
//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::moduleGetFunction(VEDAfunction* func, VEDAmodule mod, const char* name) {
	if(name == 0)	return VEDA_ERROR_INVALID_VALUE;
	LOCK();
	*func =  veo_get_sym(m_handle, mod ? mod->lib() : 0, name);
	return *func == 0 ? VEDA_ERROR_FUNCTION_NOT_FOUND : VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::moduleLoad(VEDAmodule* mod, const char* name) {
	if(name == 0)	return VEDA_ERROR_INVALID_VALUE;
	LOCK();
	auto lib = veo_load_library(m_handle, name);
	if(lib == 0)	return VEDA_ERROR_MODULE_NOT_FOUND;
	*mod = &m_modules.emplace(MAP_EMPLACE(lib, this, lib)).first->second;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::moduleUnload(VEDAmodule mod) {
	LOCK();
	CVEO(veo_unload_library(m_handle, mod->lib()));
	m_modules.erase(mod->lib());
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
// Malloc/Free
//------------------------------------------------------------------------------
void __VEDAcontext::incMemIdx(void) {
	m_memidx++;
	m_memidx = m_memidx & 0x00FFFFFF;
	if(m_memidx == 0)
		m_memidx = 1;
}

//------------------------------------------------------------------------------
VEDAdeviceptr __VEDAcontext::newVPTR(veo_ptr** ptr, const size_t size) {
	LOCK();
	if(m_ptrs.size() == 0xFFFFFF)
		return VEDA_ERROR_OUT_OF_MEMORY; // no VPTRs left

	while(m_ptrs.find(m_memidx) != m_ptrs.end())
		incMemIdx();

	VEDAptr pptr(device(), m_memidx);
	auto it = m_ptrs.emplace(MAP_EMPLACE(pptr.idx(), 0, size)).first;
	*ptr = &std::get<0>(it->second);
	incMemIdx();

	return pptr;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::free(VEDAdeviceptr vptr) {
	VEDAptr pptr(vptr);
	assert(pptr.device() == device());
	return m_ptrs.erase(pptr.idx()) == 0 ? VEDA_ERROR_UNKNOWN_VPTR : VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::getBasePtr(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr) {
	*size = 0;
	*ptr = 0;

	VEDAptr pptr(vptr);
	assert(pptr.device() == device());
	auto it = m_ptrs.find(pptr.idx());
	if(it == m_ptrs.end()) {
		TRACE("VEDAcontext::getBasePtr(%p, %p, %p)\n", *ptr, *size, vptr);
		return VEDA_ERROR_UNKNOWN_VPTR;
	}
	
	if(std::get<0>(it->second) == 0)
		CVEDA(syncPtrs());

	*ptr	= std::get<0>(it->second);
	*size	= std::get<1>(it->second);
	TRACE("VEDAcontext::getBasePtr(%p, %p, %p)\n", *ptr, *size, vptr);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::getFreeMem(size_t* free, const size_t total) {
	CVEDA(syncPtrs());

	size_t used = 0;
	for(auto& it : m_ptrs)
		used += std::get<1>(it.second);

	*free = total - used;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::syncPtrs(void) {
	TRACE("VEDAcontext::syncPtrs()\n");
	bool syn = false;

	for(auto& it : m_ptrs) {
		auto idx	= it.first;
		auto& ptr	= std::get<0>(it.second);
		auto& size	= std::get<1>(it.second);

		if(ptr == 0) {
			// if size is == 0, then no malloc call had been issued, so we need to fetch the info
			// is size is != 0, then we only need to wait till the malloc reports back the ptr
			if(size == 0) {
				VEDAptr vptr(device(), idx);
				CVEDA(memPtr(&ptr, &size, vptr.vptr(), 0));
			}
			syn = true;
		}
	}
	
	// sync all streams as we don't know which mallocs are in flight in a different stream
	if(syn)
		CVEDA(sync());
	
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memAlloc(VEDAdeviceptr* vptr, const size_t size, VEDAstream stream) {
	veo_ptr* ptr;
	*vptr = newVPTR(&ptr, size);
	TRACE("VEDAcontext::memAlloc(%p, %llu)\n", *vptr, size);
	if(size)
		CVEDA(vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEM_ALLOC), VEDAstack(ptr, VEDA_ARGS_INTENT_INOUT, sizeof(veo_ptr)), *vptr, size));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memAllocPitch(VEDAdeviceptr* vptr, size_t* pitch, const size_t w_bytes, const size_t h, const uint32_t elementSize, VEDAstream stream) {
	*pitch = w_bytes;
	return memAlloc(vptr, w_bytes * h, stream);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memFree(VEDAdeviceptr vptr, VEDAstream stream) {
	if(VEDAptr(vptr).offset() != 0)
		return VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED;

	size_t size; veo_ptr ptr;

	// get physical pointer
	auto err = getPtr(&ptr, &size, vptr);
	TRACE("VEDAcontext::memFree(%p), ptr=%p, size=%llu\n", vptr, ptr, size);
	// if(err == VEDA_ERROR_UNKNOWN_PPTR):	has already been freed on device
	// if(err == VEDA_SUCCESS):				needs to be freed from host

	// free vptr on device
	if(err == VEDA_SUCCESS)
		CVEDA(vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEM_FREE), vptr));

	// free vptr on host
	if(err == VEDA_ERROR_UNKNOWN_PPTR || err == VEDA_SUCCESS) {
		CVEDA(free(vptr));
		return VEDA_SUCCESS;
	}

	return err;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memPtr(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr, VEDAstream stream) {
	TRACE("VEDAcontext::memPtr(%p, %llu, %p)\n", *ptr, *size, vptr);
	return vedaCtxCall(this, stream, false, kernel(VEDA_KERNEL_MEM_PTR), 
		VEDAstack(ptr,  VEDA_ARGS_INTENT_INOUT, sizeof(veo_ptr)),
		VEDAstack(size, VEDA_ARGS_INTENT_INOUT, sizeof(size_t)), 
		vptr);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::getPtr(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr) {
	// does the host already know this ptr?
	CVEDA(getBasePtr(ptr, size, vptr));
	if(*ptr == 0)
		return VEDA_ERROR_UNKNOWN_PPTR;

	VEDAptr pptr(vptr);
	*ptr = *ptr + pptr.offset();

	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
// Function Calls
//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::call(VEDAfunction func, VEDAstream _stream, VEDAargs args, const bool destroyArgs, const bool checkResult) {
	LOCK();
	TRACE("VEDAcontext::call(%p (%s), %p, %i, %i)\n", func, kernelName(func), args, destroyArgs, checkResult);
	veo_thr_ctxt* str;
	CVEDA(stream(&str, _stream));
	uint64_t req = CREQ(veo_call_async(str, func, args));
	m_streams[_stream].calls.emplace_back(req, destroyArgs ? args : 0, checkResult);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::call(VEDAhost_function func, void* userData, VEDAstream _stream) {
	LOCK();
	TRACE("VEDAcontext::call(%p, %p)\n", func, userData);
	veo_thr_ctxt* str;
	CVEDA(stream(&str, _stream));
	uint64_t req = CREQ(veo_call_async_vh(str, func, userData));
	m_streams[_stream].calls.emplace_back(req, (VEDAargs)0, false);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
// Memcpy
//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memcpyD2D(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size, VEDAstream stream) {
	TRACE("VEDAcontext::memcpyD2D(%p, %p, %llu)\n", dst, src, size);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMCPY_D2D), dst, src, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memcpyD2H(void* dst, VEDAdeviceptr src, const size_t bytes, VEDAstream _stream) {
	size_t size; veo_ptr ptr;
	CVEDA(getPtr(&ptr, &size, src));
	TRACE("VEDAcontext::memcpyD2H(%p, %p, %llu), ptr=%p, size=%llu\n", dst, src, bytes, ptr, size);
	LOCK();
	veo_thr_ctxt* str;
	CVEDA(stream(&str, _stream));
	uint64_t req = CREQ(veo_async_read_mem(str, dst, ptr, bytes));
	m_streams[_stream].calls.emplace_back(req, (VEDAargs)0, false);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memcpyH2D(VEDAdeviceptr dst, const void* src, const size_t bytes, VEDAstream _stream) {
	size_t size; veo_ptr ptr;
	CVEDA(getPtr(&ptr, &size, dst));
	TRACE("VEDAcontext::memcpyH2D(%p, %p, %llu), ptr=%p, size=%llu\n", dst, src, bytes, ptr, size);
	LOCK();
	veo_thr_ctxt* str;
	CVEDA(stream(&str, _stream));
	uint64_t req = CREQ(veo_async_write_mem(str, ptr, src, bytes));
	m_streams[_stream].calls.emplace_back(req, (VEDAargs)0, false);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
// Memset
//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset(VEDAdeviceptr dst, const uint8_t value, const size_t size, VEDAstream stream) {
	TRACE("VEDAcontext::memsetU8(%p, %u, %llu)\n", dst, value, size);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U8), dst, value, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset(VEDAdeviceptr dst, const uint16_t value, const size_t size, VEDAstream stream) {
	TRACE("VEDAcontext::memsetU16(%p, %u, %llu)\n", dst, value, size);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U16), dst, value, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset(VEDAdeviceptr dst, const uint32_t value, const size_t size, VEDAstream stream) {
	TRACE("VEDAcontext::memsetU32(%p, %u, %llu)\n", dst, value, size);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U32), dst, value, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset(VEDAdeviceptr dst, const uint64_t value, const size_t size, VEDAstream stream) {
	TRACE("VEDAcontext::memsetU64(%p, %llu, %llu)\n", dst, value, size);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U64), dst, value, size);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h, VEDAstream stream) {
	TRACE("VEDAcontext::memsetU8_2D(%p, %lluu, %u, %llu)\n", dst, value, w, h);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U8_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h, VEDAstream stream) {
	TRACE("VEDAcontext::memsetU16_2D(%p, %lluu, %u, %llu)\n", dst, value, w, h);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U16_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h, VEDAstream stream) {
	TRACE("VEDAcontext::memsetU32_2D(%p, %lluu, %u, %llu)\n", dst, value, w, h);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U32_2D), pitch, value, w, h);
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h, VEDAstream stream) {
	TRACE("VEDAcontext::memsetU64_2D(%p, %lluu, %llu, %llu)\n", dst, value, w, h);
	return vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U64_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
// Sync
//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::sync(void) {
	for(size_t i = 0; i < m_streams.size(); i++)
		if(m_streams[i].ctx != 0)
			CVEDA(sync((VEDAstream)i));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::sync(VEDAstream _stream) {
	LOCK();

	veo_thr_ctxt* str;
	CVEDA(stream(&str, _stream));
	auto& calls = m_streams[_stream].calls;

	TRACE("VEDAcontext::sync(%i), calls=%llu\n", _stream, calls.size());

	auto err = [&](void) {
		for(auto& it : calls) {
			auto id = std::get<0>(it);
			auto args = std::get<1>(it);
			auto checkResult = std::get<2>(it);

			uint64_t res = 0;
			CVEO(veo_call_wait_result(str, id, &res));
			VEDAresult _res = (VEDAresult)res;

			if(checkResult)
				CVEDA(_res);

			if(args)
				CVEDA(vedaArgsDestroy(args));
		}
		return VEDA_SUCCESS;
	}();
	calls.clear();
	return err;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAcontext::query(VEDAstream _stream) {
	LOCK();
	veo_thr_ctxt* str;
	CVEDA(stream(&str, _stream));
	auto state = veo_get_context_state(str);
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
