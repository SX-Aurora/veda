#include "veda.hpp"

#define LOCK() std::lock_guard<std::mutex> lock(m_mutex)

namespace veda {
//------------------------------------------------------------------------------
// Static Inline
//------------------------------------------------------------------------------
static inline void vedaCtxCall(Context* ctx, VEDAstream stream, bool checkResults, VEDAfunction func, VEDAargs args, const int idx) {
	ctx->call(func, stream, args, true, checkResults);
}

//------------------------------------------------------------------------------
template<typename T, typename... Args>
static inline void vedaCtxCall(Context* ctx, VEDAstream stream, bool checkResults, VEDAfunction func, VEDAargs args, const int idx, const T value, Args... vargs) {
	TVEDA(vedaArgsSet(args, idx, value));
	vedaCtxCall(ctx, stream, checkResults, func, args, idx+1, vargs...);
}

//------------------------------------------------------------------------------
template<typename... Args>
static inline void vedaCtxCall(Context* ctx, VEDAstream stream, bool checkResults, VEDAfunction func, Args... vargs) {
	VEDAargs args = 0;
	TVEDA(vedaArgsCreate(&args));
	vedaCtxCall(ctx, stream, checkResults, func, args, 0, vargs...);
}

//------------------------------------------------------------------------------
// Context Class
//------------------------------------------------------------------------------
Device& 		Context::device		(void)		{	return m_device;		}
VEDAcontext_mode	Context::mode		(void) const	{	return m_mode;			}
int			Context::streamCount	(void) const	{	return (int)m_streams.size();	}

//------------------------------------------------------------------------------
Context::Context(Device& device, const VEDAcontext_mode mode) :
	m_mode	(mode),
	m_device(device),
	m_handle(0),
	m_lib	(0),
	m_memidx(1)
{
	// Create AVEO proc ----------------------------------------------------
	int cores	= std::min(device.cores(), veda::ompThreads());
	int numStreams	= 0;
	if(mode == VEDA_CONTEXT_MODE_OMP) {
		char buffer[3];
		snprintf(buffer, sizeof(buffer), "%i", cores);
		setenv("VE_OMP_NUM_THREADS", buffer, 1);
		numStreams = 1;
	} else if(mode == VEDA_CONTEXT_MODE_SCALAR) {
		setenv("VE_OMP_NUM_THREADS", "1", 1);
		numStreams = cores;
	} else {
		throw VEDA_ERROR_INVALID_VALUE;
	}
	ASSERT(numStreams);

	auto createProc = [this](void) {
		auto handle = veo_proc_create(this->device().aveoId());
		if(!handle)
			throw VEDA_ERROR_CANNOT_CREATE_CONTEXT;
		return handle;	
	};

	m_handle = createProc();
	ASSERT(m_handle);

	// Load STDLib ---------------------------------------------------------
	m_kernels.resize(VEDA_KERNEL_CNT);

	m_lib = moduleLoad(veda::stdLib());
	for(int i = 0; i < VEDA_KERNEL_CNT; i++)
		m_kernels[i] = moduleGetFunction(m_lib, kernelName((Kernel)i));

	// Create Streams ------------------------------------------------------
	m_streams.resize(numStreams);
	for(auto& stream : m_streams) {
		ASSERT(stream.ctx == 0);
		stream.ctx = veo_context_open(m_handle);
		if(stream.ctx == 0)
			throw VEDA_ERROR_CANNOT_CREATE_STREAM;
		stream.calls.reserve(128);
		ASSERT(stream.calls.empty());
	}
}

//------------------------------------------------------------------------------
Context::~Context(void) noexcept(false) {
	syncPtrs();
	for(auto& it : m_ptrs) {
		auto idx	= it.first;
		auto& size	= std::get<1>(it.second);

		Ptr vptr(device().vedaId(), idx);
		printf("[VEDA ERROR]: VEDAdeviceptr %p with size %lluB has not been freed!\n", vptr.vptr(), size);
	}

	if(m_handle)
		TVEO(veo_proc_destroy(m_handle));

	m_streams.clear();	// don't need to be destroyed
	m_modules.clear();	// don't need to be destroyed
	m_kernels.clear();	// don't need to be destroyed
}

//------------------------------------------------------------------------------
size_t Context::memUsed(void) {
	syncPtrs();
	size_t used = 0;
	for(auto& it : m_ptrs)
		used += std::get<1>(it.second);
	return used;
}

//------------------------------------------------------------------------------
void Context::memReport(void) {
	syncPtrs();
	size_t total	= device().memorySize();
	size_t used	= memUsed();

	printf("# VE#%i %.2f/%.2fGB\n", device(), used/(1024.0*1024.0*1024.0), total/(1024.0*1024.0*1024.0));
	for(auto& it : m_ptrs) {
		Ptr vptr(device().vedaId(), it.first);
		auto ptr	= std::get<0>(it.second);
		auto size	= std::get<1>(it.second);
		printf("%p/%p %lluB\n", vptr.vptr(), ptr, size);
	}
}

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
veo_thr_ctxt* Context::stream(const VEDAstream stream) {
	if(stream < 0 || stream >= m_streams.size() || m_streams[stream].ctx == 0)
		throw VEDA_ERROR_UNKNOWN_STREAM;
	return m_streams[stream].ctx;
}

//------------------------------------------------------------------------------
// Kernels
//------------------------------------------------------------------------------
const char* Context::kernelName(const Kernel k) const {
	switch(k) {
		case VEDA_KERNEL_MEMSET_U8:	return "veda_memset_u8";
		case VEDA_KERNEL_MEMSET_U16:	return "veda_memset_u16";
		case VEDA_KERNEL_MEMSET_U32:	return "veda_memset_u32";
		case VEDA_KERNEL_MEMSET_U64:	return "veda_memset_u64";
		case VEDA_KERNEL_MEMSET_U8_2D:	return "veda_memset_u8_2d";
		case VEDA_KERNEL_MEMSET_U16_2D:	return "veda_memset_u16_2d";
		case VEDA_KERNEL_MEMSET_U32_2D:	return "veda_memset_u32_2d";
		case VEDA_KERNEL_MEMSET_U64_2D:	return "veda_memset_u64_2d";
		case VEDA_KERNEL_MEMCPY_D2D:	return "veda_memcpy_d2d";
		case VEDA_KERNEL_MEM_ALLOC:	return "veda_mem_alloc";
		case VEDA_KERNEL_MEM_FREE:	return "veda_mem_free";
		case VEDA_KERNEL_MEM_PTR:	return "veda_mem_ptr";
	}

	throw VEDA_ERROR_UNKNOWN_KERNEL;
}

//------------------------------------------------------------------------------
const char* Context::kernelName(VEDAfunction func) const {
	int idx = 0;
	for(; idx < VEDA_KERNEL_CNT; idx++) {
		if(m_kernels[idx] == func)
			break;
	}

	switch(idx) {
		case VEDA_KERNEL_MEMSET_U8:	return "VEDA_KERNEL_MEMSET_U8";
		case VEDA_KERNEL_MEMSET_U16:	return "VEDA_KERNEL_MEMSET_U16";
		case VEDA_KERNEL_MEMSET_U32:	return "VEDA_KERNEL_MEMSET_U32";
		case VEDA_KERNEL_MEMSET_U64:	return "VEDA_KERNEL_MEMSET_U64";
		case VEDA_KERNEL_MEMSET_U8_2D:	return "VEDA_KERNEL_MEMSET_U8_2D";
		case VEDA_KERNEL_MEMSET_U16_2D:	return "VEDA_KERNEL_MEMSET_U16_2D";
		case VEDA_KERNEL_MEMSET_U32_2D:	return "VEDA_KERNEL_MEMSET_U32_2D";
		case VEDA_KERNEL_MEMSET_U64_2D:	return "VEDA_KERNEL_MEMSET_U64_2D";
		case VEDA_KERNEL_MEMCPY_D2D:	return "VEDA_KERNEL_MEMCPY_D2D";
		case VEDA_KERNEL_MEM_ALLOC:	return "VEDA_KERNEL_MEM_ALLOC";
		case VEDA_KERNEL_MEM_FREE:	return "VEDA_KERNEL_MEM_FREE";
		case VEDA_KERNEL_MEM_PTR:	return "VEDA_KERNEL_MEM_PTR";
	}

	return "USER_KERNEL";
}

//------------------------------------------------------------------------------
VEDAfunction Context::kernel(Kernel kernel) const {
	if(kernel < 0 || kernel >= VEDA_KERNEL_CNT)
		throw VEDA_ERROR_UNKNOWN_KERNEL;
	return m_kernels[kernel];
}

//------------------------------------------------------------------------------
// Modules
//------------------------------------------------------------------------------
VEDAfunction Context::moduleGetFunction(Module* mod, const char* name) {
	if(name == 0)
		throw VEDA_ERROR_INVALID_VALUE;
	LOCK();
	auto func = veo_get_sym(m_handle, mod ? mod->lib() : 0, name);
	if(func == 0)
		throw VEDA_ERROR_FUNCTION_NOT_FOUND;
	return func;
}

//------------------------------------------------------------------------------
Module* Context::moduleLoad(const char* name) {
	if(name == 0)
		throw VEDA_ERROR_INVALID_VALUE;
	LOCK();
	auto lib = veo_load_library(m_handle, name);
	if(lib == 0)
		throw VEDA_ERROR_MODULE_NOT_FOUND;
	return &m_modules.emplace(MAP_EMPLACE(lib, this, lib)).first->second;
}

//------------------------------------------------------------------------------
void Context::moduleUnload(const Module* mod) {
	LOCK();
	TVEO(veo_unload_library(m_handle, mod->lib()));
	m_modules.erase(mod->lib());
}

//------------------------------------------------------------------------------
// Malloc/Free
//------------------------------------------------------------------------------
void Context::incMemIdx(void) {
	m_memidx++;
	m_memidx = m_memidx & 0x00FFFFFF;
	if(m_memidx == 0)
		m_memidx = 1;
}

//------------------------------------------------------------------------------
VEDAdeviceptr Context::newVPTR(veo_ptr** ptr, const size_t size) {
	LOCK();
	if(m_ptrs.size() == 0xFFFFFF)
		throw VEDA_ERROR_OUT_OF_MEMORY; // no VPTRs left

	while(m_ptrs.find(m_memidx) != m_ptrs.end())
		incMemIdx();

	Ptr pptr(device().vedaId(), m_memidx);
	auto it = m_ptrs.emplace(MAP_EMPLACE(pptr.idx(), 0, size)).first;
	*ptr = &std::get<0>(it->second);
	incMemIdx();
	return pptr;
}

//------------------------------------------------------------------------------
void Context::free(VEDAdeviceptr vptr) {
	Ptr pptr(vptr);
	ASSERT(pptr.device() == device().vedaId());
	if(m_ptrs.erase(pptr.idx()) == 0)
		throw VEDA_ERROR_UNKNOWN_VPTR;
}

//------------------------------------------------------------------------------
Context::PtrTuple Context::getBasePtr(VEDAdeviceptr vptr) {
	Ptr pptr(vptr);
	ASSERT(pptr.device() == device().vedaId());
	auto it = m_ptrs.find(pptr.idx());
	if(it == m_ptrs.end())
		throw VEDA_ERROR_UNKNOWN_VPTR;
	if(std::get<0>(it->second) == 0)
		syncPtrs();
	return it->second;
}

//------------------------------------------------------------------------------
void Context::syncPtrs(void) {
	bool syn = false;

	for(auto& it : m_ptrs) {
		auto idx	= it.first;
		auto& ptr	= std::get<0>(it.second);
		auto& size	= std::get<1>(it.second);

		if(ptr == 0) {
			// if size is == 0, then no malloc call had been issued, so we need to fetch the info
			// is size is != 0, then we only need to wait till the malloc reports back the ptr
			if(size == 0) {
				Ptr vptr(device().vedaId(), idx);
				vedaCtxCall(this, 0, false, kernel(VEDA_KERNEL_MEM_PTR), 
					VEDAstack(&ptr,  VEDA_ARGS_INTENT_OUT, sizeof(veo_ptr)),
					VEDAstack(&size, VEDA_ARGS_INTENT_OUT, sizeof(size_t)), 
					vptr);
			}
			syn = true;
		}
	}
	
	// sync all streams as we don't know which mallocs are in flight in a different stream
	if(syn)
		sync();
}

//------------------------------------------------------------------------------
VEDAdeviceptr Context::memAlloc(const size_t size, VEDAstream stream) {
	veo_ptr* ptr;
	auto vptr = newVPTR(&ptr, size);
	if(size)
		vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEM_ALLOC), VEDAstack(ptr, VEDA_ARGS_INTENT_INOUT, sizeof(veo_ptr)), vptr, size);
	return vptr;
}

//------------------------------------------------------------------------------
Context::PtrTuple Context::memAllocPitch(const size_t w_bytes, const size_t h, const uint32_t elementSize, VEDAstream stream) {
	return std::make_tuple(memAlloc(w_bytes * h, stream), w_bytes);
}

//------------------------------------------------------------------------------
void Context::memFree(VEDAdeviceptr vptr, VEDAstream stream) {
	if(Ptr(vptr).offset() != 0)
		throw VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED;

	// get physical pointer
	auto res = getPtr(vptr);
	if(std::get<0>(res))
		vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEM_FREE), vptr);
	free(vptr);
}

//------------------------------------------------------------------------------
Context::PtrTuple Context::getPtr(VEDAdeviceptr vptr) {
	// does the host already know this ptr?
	auto res = getBasePtr(vptr);
	if(std::get<0>(res) == 0)
		return res;

	Ptr pptr(vptr); // apply offset
	std::get<0>(res) += pptr.offset();
	
	return res;
}

//------------------------------------------------------------------------------
// Function Calls
//------------------------------------------------------------------------------
void Context::call(VEDAfunction func, VEDAstream _stream, VEDAargs args, const bool destroyArgs, const bool checkResult) {
	LOCK();
	auto str	= stream(_stream);
	uint64_t req	= CREQ(veo_call_async(str, func, args));
	m_streams[_stream].calls.emplace_back(req, checkResult);
	if(destroyArgs)
		TVEDA(vedaArgsDestroy(args));
}

//------------------------------------------------------------------------------
void Context::call(VEDAhost_function func, void* userData, VEDAstream _stream) {
	LOCK();
	auto str	= stream(_stream);
	uint64_t req	= CREQ(veo_call_async_vh(str, func, userData));
	m_streams[_stream].calls.emplace_back(req, false);
}

//------------------------------------------------------------------------------
// Memcpy
//------------------------------------------------------------------------------
void Context::memcpyD2D(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMCPY_D2D), dst, src, size);
}

//------------------------------------------------------------------------------
void Context::memcpyD2H(void* dst, VEDAdeviceptr src, const size_t bytes, VEDAstream _stream) {
	auto ptr = std::get<0>(getPtr(src)); ASSERT(ptr);
	LOCK();
	auto str	= stream(_stream);
	uint64_t req	= CREQ(veo_async_read_mem(str, dst, ptr, bytes));
	m_streams[_stream].calls.emplace_back(req, false);
}

//------------------------------------------------------------------------------
void Context::memcpyH2D(VEDAdeviceptr dst, const void* src, const size_t bytes, VEDAstream _stream) {
	auto ptr = std::get<0>(getPtr(dst)); ASSERT(ptr);
	LOCK();
	auto str	= stream(_stream);
	uint64_t req	= CREQ(veo_async_write_mem(str, ptr, src, bytes));
	m_streams[_stream].calls.emplace_back(req, false);
}

//------------------------------------------------------------------------------
// Memset
//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint8_t value, const size_t size, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U8), dst, value, size);
}

//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint16_t value, const size_t size, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U16), dst, value, size);
}

//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint32_t value, const size_t size, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U32), dst, value, size);
}

//------------------------------------------------------------------------------
void Context::memset(VEDAdeviceptr dst, const uint64_t value, const size_t size, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U64), dst, value, size);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U8_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U16_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U32_2D), pitch, value, w, h);
}

//------------------------------------------------------------------------------
void Context::memset2D(VEDAdeviceptr dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h, VEDAstream stream) {
	vedaCtxCall(this, stream, true, kernel(VEDA_KERNEL_MEMSET_U64_2D), dst, pitch, value, w, h);
}

//------------------------------------------------------------------------------
// Sync
//------------------------------------------------------------------------------
void Context::sync(void) {
	for(size_t i = 0; i < m_streams.size(); i++)
		if(m_streams[i].ctx != 0)
			sync((VEDAstream)i);
}

//------------------------------------------------------------------------------
void Context::sync(VEDAstream _stream) {
	LOCK();

	auto str	= stream(_stream);
	auto& calls	= m_streams[_stream].calls;

	for(auto& it : calls) {
		auto id			= std::get<0>(it);
		auto checkResult	= std::get<1>(it);
		uint64_t res		= 0;
		TVEO(veo_call_wait_result(str, id, &res));
		VEDAresult _res = (VEDAresult)res;

		if(checkResult && _res != VEDA_SUCCESS)
			throw _res;
	}
	
	calls.clear();
}

//------------------------------------------------------------------------------
VEDAresult Context::query(VEDAstream _stream) {
	LOCK();
	auto state = veo_get_context_state(stream(_stream));
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
}
