#include "internal.h"

#define LOCK(X) std::lock_guard<std::mutex> __lock__(X)

namespace veda {
//------------------------------------------------------------------------------
// Static Inline
//------------------------------------------------------------------------------
static inline void vedaCtxCall(Context* ctx, VEDAstream stream, const bool checkResult, uint64_t* result, VEDAfunction func, VEDAargs args, const int idx) {
	ctx->call(func, stream, args, true, checkResult, result);
}

//------------------------------------------------------------------------------
template<typename T, typename... Args>
static inline void vedaCtxCall(Context* ctx, VEDAstream stream, const bool checkResult, uint64_t* result, VEDAfunction func, VEDAargs args, const int idx, const T value, Args... vargs) {
	TVEDA(vedaArgsSet(args, idx, value));
	vedaCtxCall(ctx, stream, checkResult, result, func, args, idx+1, vargs...);
}

//------------------------------------------------------------------------------
template<typename... Args>
static inline void vedaCtxCall(Context* ctx, VEDAstream stream, const bool checkResult, uint64_t* result, VEDAfunction func, Args... vargs) {
	VEDAargs args = 0;
	TVEDA(vedaArgsCreate(&args));
	vedaCtxCall(ctx, stream, checkResult, result, func, args, 0, vargs...);
}

//------------------------------------------------------------------------------
// Context Class
//------------------------------------------------------------------------------
Device& 		Context::device		(void)		{	return m_device;		}
VEDAcontext_mode	Context::mode		(void) const	{	return m_mode;			}
bool			Context::isActive	(void) const	{	return m_handle != 0;		}
int			Context::streamCount	(void) const	{	return (int)m_streams.size();	}

//------------------------------------------------------------------------------
void Context::setMemOverride(VEDAdeviceptr vptr) {
	if(vptr != 0) {
		if(VEDA_GET_OFFSET(vptr))			VEDA_THROW(VEDA_ERROR_OFFSET_NOT_ALLOWED);
		if(VEDA_GET_DEVICE(vptr) != device().vedaId())	VEDA_THROW(VEDA_ERROR_INVALID_DEVICE);
	}
	m_memOverride = vptr;
}

//------------------------------------------------------------------------------
Context::Context(Device& device) :
	m_mode		(VEDA_CONTEXT_MODE_OMP),
	m_device	(device),
	m_handle	(0),
	m_lib		(0),
	m_memidx	(1),
	m_memOverride	(0)
{}

//------------------------------------------------------------------------------
VEDAhmemptr Context::hmemAlloc(const size_t size) {
	VEDAhmemptr ptr;
	if(veo_alloc_hmem(m_handle, (void**)&ptr, size) != 0)
		VEDA_THROW(VEDA_ERROR_OUT_OF_MEMORY);
	return ptr;
}

//------------------------------------------------------------------------------
size_t Context::memUsed(void) {
	LOCK(mutex_ptrs);
	syncPtrs();

	size_t used = 0;
	for(auto&& [idx, info] : m_ptrs)
		used += info->size;
	
	return used;
}

//------------------------------------------------------------------------------
void Context::memReport(void) {	
	if(!isActive())
		return;

	size_t total	= device().memorySize();
	size_t used	= memUsed();

	LOCK(mutex_ptrs);
	printf("# VE#%i %.2f/%.2fGB\n", device().vedaId(), used/(1024.0*1024.0*1024.0), total/(1024.0*1024.0*1024.0));
	for(auto&& [idx, info] : m_ptrs) {
		auto vptr = VEDA_SET_PTR(device().vedaId(), idx, 0);
		printf("%p/%p %lluB\n", vptr, info->ptr, info->size);
	}
	printf("\n");
}

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
Stream& Context::stream(const VEDAstream stream) {
	if(stream < 0 || stream >= m_streams.size())	VEDA_THROW(VEDA_ERROR_UNKNOWN_STREAM);
	auto& ref = m_streams[stream];
	if(ref.ctx == 0)				VEDA_THROW(VEDA_ERROR_UNKNOWN_STREAM);
	return ref;
}

//------------------------------------------------------------------------------
// Kernels
//------------------------------------------------------------------------------
const char* Context::kernelName(const Kernel k) const {
	switch(k) {
		case VEDA_KERNEL_MEMCPY_D2D:		return "veda_memcpy_d2d";
		case VEDA_KERNEL_MEMSET_U128:		return "veda_memset_u128";
		case VEDA_KERNEL_MEMSET_U128_2D:	return "veda_memset_u128_2d";
		case VEDA_KERNEL_MEMSET_U16:		return "veda_memset_u16";
		case VEDA_KERNEL_MEMSET_U16_2D:		return "veda_memset_u16_2d";
		case VEDA_KERNEL_MEMSET_U32:		return "veda_memset_u32";
		case VEDA_KERNEL_MEMSET_U32_2D:		return "veda_memset_u32_2d";
		case VEDA_KERNEL_MEMSET_U64:		return "veda_memset_u64";
		case VEDA_KERNEL_MEMSET_U64_2D:		return "veda_memset_u64_2d";
		case VEDA_KERNEL_MEMSET_U8:		return "veda_memset_u8";
		case VEDA_KERNEL_MEMSET_U8_2D:		return "veda_memset_u8_2d";
		case VEDA_KERNEL_MEM_ALLOC:		return "veda_mem_alloc";
		case VEDA_KERNEL_MEM_FREE:		return "veda_mem_free";
		case VEDA_KERNEL_MEM_PTR:		return "veda_mem_ptr";
		case VEDA_KERNEL_MEM_PTR_VPTR:		return "veda_mem_ptr_vptr";
		case VEDA_KERNEL_MEM_SIZE:		return "veda_mem_size";
		case VEDA_KERNEL_MEM_SWAP:		return "veda_mem_swap";
	}

	VEDA_THROW(VEDA_ERROR_UNKNOWN_KERNEL);
}

//------------------------------------------------------------------------------
const char* Context::kernelName(VEDAfunction func) const {
	int idx = 0;
	for(; idx < VEDA_KERNEL_CNT; idx++)
		if(m_kernels[idx] == func)
			break;

	switch(idx) {
		case VEDA_KERNEL_MEMCPY_D2D:	return "VEDA_KERNEL_MEMCPY_D2D";
		case VEDA_KERNEL_MEMSET_U16:	return "VEDA_KERNEL_MEMSET_U16";
		case VEDA_KERNEL_MEMSET_U16_2D:	return "VEDA_KERNEL_MEMSET_U16_2D";
		case VEDA_KERNEL_MEMSET_U32:	return "VEDA_KERNEL_MEMSET_U32";
		case VEDA_KERNEL_MEMSET_U32_2D:	return "VEDA_KERNEL_MEMSET_U32_2D";
		case VEDA_KERNEL_MEMSET_U64:	return "VEDA_KERNEL_MEMSET_U64";
		case VEDA_KERNEL_MEMSET_U64_2D:	return "VEDA_KERNEL_MEMSET_U64_2D";
		case VEDA_KERNEL_MEMSET_U8:	return "VEDA_KERNEL_MEMSET_U8";
		case VEDA_KERNEL_MEMSET_U8_2D:	return "VEDA_KERNEL_MEMSET_U8_2D";
		case VEDA_KERNEL_MEM_ALLOC:	return "VEDA_KERNEL_MEM_ALLOC";
		case VEDA_KERNEL_MEM_FREE:	return "VEDA_KERNEL_MEM_FREE";
		case VEDA_KERNEL_MEM_PTR:	return "VEDA_KERNEL_MEM_PTR";
		case VEDA_KERNEL_MEM_PTR_VPTR:	return "VEDA_KERNEL_MEM_PTR_VPTR";
		case VEDA_KERNEL_MEM_SIZE:	return "VEDA_KERNEL_MEM_SIZE";
		case VEDA_KERNEL_MEM_SWAP:	return "VEDA_KERNEL_MEM_SWAP";
	}

	return "USER_KERNEL";
}

//------------------------------------------------------------------------------
VEDAfunction Context::kernel(Kernel kernel) const {
	if(kernel < 0 || kernel >= VEDA_KERNEL_CNT)
		VEDA_THROW(VEDA_ERROR_UNKNOWN_KERNEL);
	return m_kernels[kernel];
}

//------------------------------------------------------------------------------
// Modules
//------------------------------------------------------------------------------
VEDAfunction Context::moduleGetFunction(Module* mod, const char* name) {
	if(name == 0)	VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	auto func = veo_get_sym(m_handle, mod ? mod->lib() : 0, name);
	if(func == 0)	VEDA_THROW(VEDA_ERROR_FUNCTION_NOT_FOUND);
	return func;
}

//------------------------------------------------------------------------------
Module* Context::moduleLoad(const char* name) {
	if(name == 0 || !*name)	VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	auto lib = veo_load_library(m_handle, name);
	if(lib == 0)		VEDA_THROW(VEDA_ERROR_MODULE_NOT_FOUND);
	LOCK(mutex_modules);
	return &m_modules.emplace(MAP_EMPLACE(lib, this, lib)).first->second;
}

//------------------------------------------------------------------------------
void Context::moduleUnload(const Module* mod) {
	TVEO(veo_unload_library(m_handle, mod->lib()));
	LOCK(mutex_modules);
	m_modules.erase(mod->lib());
}

//------------------------------------------------------------------------------
// Malloc/Free
//------------------------------------------------------------------------------
void Context::incMemIdx(void) {
	m_memidx++;
	m_memidx = m_memidx & VEDA_CNT_IDX;
	if(m_memidx == 0)
		m_memidx = 1;
}

//------------------------------------------------------------------------------
void Context::syncPtrs(void) {
	bool syn = false;

	// Don't lock mutex_ptrs here, as ALL calling functions do this on behalf of this
	for(auto&& [idx, info] : m_ptrs) {
		if(info->ptr == 0) {
			// if size is == 0, then no malloc call had been issued, so we need to fetch the info
			// is size is != 0, then we only need to wait till the malloc reports back the ptr
			if(info->size == 0) {
				auto vptr = VEDA_SET_PTR(device().vedaId(), idx, 0);
				vedaCtxCall(this, 0, false, (uint64_t*)&info->ptr,  kernel(VEDA_KERNEL_MEM_PTR),  vptr);
				vedaCtxCall(this, 0, false, (uint64_t*)&info->size, kernel(VEDA_KERNEL_MEM_SIZE), vptr);
			}
			syn = true;
		}
	}
	
	// sync all streams as we don't know which mallocs are in flight in a different stream
	if(syn)
		sync();
}

//------------------------------------------------------------------------------
VEDAdeviceptr Context::memAlloc(const size_t size, VEDAstream _stream) {
	if(m_memOverride)
		syncPtrs();

	LOCK(mutex_ptrs);

	// Check if VPTRs are left ---------------------------------------------
	if(m_ptrs.size() == VEDA_CNT_IDX)
		VEDA_THROW(VEDA_ERROR_OUT_OF_MEMORY);

	// Override pointer? ---------------------------------------------------
	if(m_memOverride) {
		auto dev	= VEDA_GET_DEVICE(m_memOverride);	assert(dev == device().vedaId());
		auto idx	= VEDA_GET_IDX(m_memOverride);
		m_memOverride	= 0;
		auto it		= m_ptrs.find(idx);
		
		if(it == m_ptrs.end())	VEDA_THROW(VEDA_ERROR_UNKNOWN_VPTR);
		auto info = it->second;
		if(info->size != size)	VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
		if(info->ptr  == 0)	VEDA_THROW(VEDA_ERROR_UNKNOWN_PPTR);

		return VEDA_SET_PTR(dev, idx, 0);
	}

	// Find free idx -------------------------------------------------------
	while(m_ptrs.find(m_memidx) != m_ptrs.end())
		incMemIdx();

	auto idx  = m_memidx;
	auto info = m_ptrs.emplace(MAP_EMPLACE(idx, new VEDAdeviceptrInfo(0, size))).first->second;
	auto vptr = VEDA_SET_PTR(device().vedaId(), idx, 0);

	incMemIdx();

	if(size) {
		veo_req_block_begin(s.ctx);
		{
			auto& s		= stream(_stream);
			uint64_t req	= CREQ(veo_alloc_mem_async(s.ctx, size));
			LOCK(s.mutex);
			s.calls.emplace_back(req, false, (uint64*)0);
		}

		vedaCtxCall(this, _stream, false, (uint64_t*)&info->ptr, kernel(VEDA_KERNEL_MEM_PTR_VPTR), vptr, size);
		veo_req_block_end(s.ctx);
	}

	return vptr;
}

//------------------------------------------------------------------------------
Context::VPtrTuple Context::memAllocPitch(const size_t w_bytes, const size_t h, const uint32_t elementSize, VEDAstream stream) {
	return std::make_tuple(memAlloc(w_bytes * h, stream), w_bytes);
}

//------------------------------------------------------------------------------
void Context::memSwap(VEDAdeviceptr A, VEDAdeviceptr B, VEDAstream stream) {
	LOCK(mutex_ptrs);
	auto get = [this](VEDAdeviceptr x) {
		auto it = m_ptrs.find(VEDA_GET_IDX(x));
		if(it == m_ptrs.end())
			VEDA_THROW(VEDA_ERROR_UNKNOWN_VPTR);
		return it;
	};

	std::swap(get(A)->second, get(B)->second);
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEM_SWAP), A, B);
}

//------------------------------------------------------------------------------
void Context::memFree(VEDAdeviceptr vptr, VEDAstream stream) {
	ASSERT(VEDA_GET_DEVICE(vptr) == device().vedaId());

	// If this context is not active, we don't care about still pending frees.
	if(!isActive())
		return;

	if(VEDA_GET_OFFSET(vptr) != 0)
		VEDA_THROW(VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED);

	LOCK(mutex_ptrs);
	auto it = m_ptrs.find(VEDA_GET_IDX(vptr));
	if(it == m_ptrs.end())
		VEDA_THROW(VEDA_ERROR_UNKNOWN_VPTR);
	
	auto info = it->second;
	
	/** This is a special case. When we issue AsyncMalloc and immediately
	 * do AsyncFree, then the data might not have arrived on the host yet
	 * causing a Segfault */
	if(info->ptr == 0 && info->size != 0)
		sync();

	if(info->size)
		vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEM_FREE), vptr);

	delete info;
	m_ptrs.erase(it);
}

//------------------------------------------------------------------------------
VEDAdeviceptrInfo Context::getPtr(VEDAdeviceptr vptr) {
	ASSERT(VEDA_GET_DEVICE(vptr) == device().vedaId());
	
	LOCK(mutex_ptrs);
	auto it = m_ptrs.find(VEDA_GET_IDX(vptr));
	if(it == m_ptrs.end())
		VEDA_THROW(VEDA_ERROR_UNKNOWN_VPTR);
	
	auto info = it->second;

	if(info->ptr == 0)	syncPtrs();
	if(info->ptr == 0)	return *info;

	return {((char*)info->ptr) + VEDA_GET_OFFSET(vptr), info->size};
}

//------------------------------------------------------------------------------
// Function Calls
//------------------------------------------------------------------------------
void Context::call(VEDAfunction func, VEDAstream _stream, VEDAargs args, const bool destroyArgs, const bool checkResult, uint64_t* result) {
	auto& s		= stream(_stream);
	uint64_t req	= CREQ(veo_call_async(s.ctx, func, args));
	if(destroyArgs)
		TVEDA(vedaArgsDestroy(args));

	LOCK(s.mutex);
	s.calls.emplace_back(req, checkResult, result);
}

//------------------------------------------------------------------------------
void Context::call(VEDAhost_function func, VEDAstream _stream, void* userData, const bool checkResult, uint64_t* result) {
	auto& s		= stream(_stream);
	uint64_t req	= CREQ(veo_call_async_vh(s.ctx, func, userData));

	LOCK(s.mutex);
	s.calls.emplace_back(req, checkResult, result);
}

//------------------------------------------------------------------------------
// Memcpy
//------------------------------------------------------------------------------
void Context::memcpyD2D(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size, VEDAstream stream) {
	if(!dst || !src)
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMCPY_D2D), dst, src, size);
}

//------------------------------------------------------------------------------
void Context::memcpyD2H(void* dst, VEDAdeviceptr src, const size_t bytes, VEDAstream _stream) {
	if(!dst || !src)
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);

	auto info	= getPtr(src);
	auto ptr	= info.ptr;
	auto size	= info.size;
	THROWIF(ptr == 0 || size == 0, "Uninitialized vptr: %p, ptr: %p, size: %llu", src, ptr, size);
	if((bytes + VEDA_GET_OFFSET(src)) > size)
		VEDA_THROW(VEDA_ERROR_OUT_OF_BOUNDS);

	auto& s		= stream(_stream);
	uint64_t req	= CREQ(veo_async_read_mem(s.ctx, dst, (veo_ptr)ptr, bytes));

	LOCK(s.mutex);
	s.calls.emplace_back(req, false, (uint64_t*)0);
}

//------------------------------------------------------------------------------
void Context::memcpyH2D(VEDAdeviceptr dst, const void* src, const size_t bytes, VEDAstream _stream) {
	if(!dst || !src)
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);

	auto res	= getPtr(dst);
	auto ptr	= res.ptr;	ASSERT(ptr);
	auto size	= res.size;	ASSERT(size);
	if((bytes + VEDA_GET_OFFSET(dst)) > size)
		VEDA_THROW(VEDA_ERROR_OUT_OF_BOUNDS);

	auto& s		= stream(_stream);
	uint64_t req	= CREQ(veo_async_write_mem(s.ctx, (veo_ptr)ptr, src, bytes));

	LOCK(s.mutex);
	s.calls.emplace_back(req, false, (uint64_t*)0);
}

//------------------------------------------------------------------------------
// Memset
//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint8_t value, const size_t cnt, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U8), dst, value, cnt);
}

//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint16_t value, const size_t cnt, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U16), dst, value, cnt);
}

//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint32_t value, const size_t cnt, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U32), dst, value, cnt);
}

//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint64_t value, const size_t cnt, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U64), dst, value, cnt);
}

//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint64_t x, const uint64_t y, const size_t cnt, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U128), dst, x, y, cnt);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U8_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U16_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U32_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U64_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint64_t x, const uint64_t y, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, 0, kernel(VEDA_KERNEL_MEMSET_U128_2D), dst, pitch, x, y, w, h);
}

//------------------------------------------------------------------------------
void Context::sync(void) {
	for(size_t i = 0; i < m_streams.size(); i++)
		if(m_streams[i].ctx != 0)
			sync((VEDAstream)i);
}

//------------------------------------------------------------------------------
void Context::sync(VEDAstream _stream) {
	auto& s = stream(_stream);

	LOCK(s.mutex);
	for(auto&& [id, checkResult, result] : s.calls) {
		uint64_t res = 0;
		TVEO(veo_call_wait_result(s.ctx, id, &res));

		if(result)
			*result = res;
		
		if(checkResult) {
			auto veda = (VEDAresult)res;
			if(veda != VEDA_SUCCESS)
				VEDA_THROW(veda);
		}
	}
	
	s.calls.clear();
}

//------------------------------------------------------------------------------
VEDAresult Context::query(VEDAstream _stream) {
	auto state = veo_get_context_state(stream(_stream).ctx);
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
void Context::init(const VEDAcontext_mode mode) {
	if(isActive())
		VEDA_THROW(VEDA_ERROR_CANNOT_CREATE_CONTEXT);

	// Set Class Variables -------------------------------------------------
	m_mode = mode;

	// Create AVEO proc ----------------------------------------------------
	int numStreams	= 0;
	int cores	= device().cores();
	if(auto omp = veda::ompThreads())
		cores = std::min(cores, omp);

	// Calculate the number of VEDA SM based on the VEDA context mode.
	if(mode == VEDA_CONTEXT_MODE_OMP) {
		char buffer[3];
		snprintf(buffer, sizeof(buffer), "%i", cores);
		setenv("VE_OMP_NUM_THREADS", buffer, 1);
		numStreams = 1;
	} else if(mode == VEDA_CONTEXT_MODE_SCALAR) {
		setenv("VE_OMP_NUM_THREADS", "1", 1);
		numStreams = cores;
	} else {
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	}
	ASSERT(numStreams);

	// VE process is created and started on the VE device.
	m_handle = veo_proc_create(this->device().aveoId());
	if(!m_handle)
		VEDA_THROW(VEDA_ERROR_CANNOT_CREATE_CONTEXT);

	// Load STDLib ---------------------------------------------------------
	m_kernels.resize(VEDA_KERNEL_CNT);

	// Load the VEDA kernel module on the VE device memory.
	m_lib = moduleLoad(veda::stdLib());
	for(int i = 0; i < VEDA_KERNEL_CNT; i++)
		m_kernels[i] = moduleGetFunction(m_lib, kernelName((Kernel)i));

	// Create Streams ------------------------------------------------------
	m_streams.resize(numStreams);
	for(auto& stream : m_streams) {
		ASSERT(stream.ctx == 0);
		// Create a new AVEO context, a pseudo thread and corresponding
		// VE thread for the context.
		stream.ctx = veo_context_open(m_handle);
		if(stream.ctx == 0)
			VEDA_THROW(VEDA_ERROR_CANNOT_CREATE_STREAM);
		stream.calls.reserve(128);
		ASSERT(stream.calls.empty());
	}
}

//------------------------------------------------------------------------------
void Context::destroy(void) {
	if(!isActive())
		VEDA_THROW(VEDA_ERROR_CONTEXT_IS_DESTROYED);

	LOCK(mutex_ptrs);
	syncPtrs();

	if(veda::isMemTrace()) {
		for(auto&& [idx, info] : m_ptrs) {
			auto vptr = (VEDAdeviceptr)(VEDA_SET_PTR(device().vedaId(), idx, 0));
			printf("[VEDA ERROR]: VEDAdeviceptr %p with size %lluB has not been freed!\n", vptr, info->size);
		}
	}
	
	if(m_handle) {
		TVEO(veo_proc_destroy(m_handle));
		m_handle = 0;
	}

	m_streams.clear();	// don't need to be destroyed
	m_modules.clear();	// don't need to be destroyed
	m_kernels.clear();	// don't need to be destroyed
	for(auto&& [idx, info] : m_ptrs)
		delete info;
	m_ptrs.clear();
	m_lib		= 0;
	m_mode		= VEDA_CONTEXT_MODE_OMP;
	m_memidx	= 0;
}

//------------------------------------------------------------------------------
void Context::hmemcpyD2D(VEDAhmemptr dst, VEDAhmemptr src, const size_t size) {

}

//------------------------------------------------------------------------------
void Context::hmemcpyD2H(void* dst, VEDAhmemptr src, const size_t size) {

}

//------------------------------------------------------------------------------
void Context::hmemcpyH2D(VEDAhmemptr dst, const void* src, const size_t size) {
	
}

//------------------------------------------------------------------------------
}
