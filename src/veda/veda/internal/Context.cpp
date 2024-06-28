#include <veda/internal/api.h>

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
// Context Class
//------------------------------------------------------------------------------
Device& 		Context::device		(void)		{	return m_device;		}
VEDAcontext_mode	Context::mode		(void) const	{	return m_mode;			}
bool			Context::isActive	(void) const	{	return m_handle != 0;		}
int			Context::aveoProcId	(void) const	{	return m_aveoProcId;		}
int			Context::streamCnt	(void) const	{	return (int)m_streams.size();	}
void			Context::sync		(void)		{	sync(-1);			}

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
	m_kernels	(VEDA_KERNEL_CNT),
	m_device	(device),
	m_handle	(0),
	m_aveoProcId	(-1),
	m_lib		(0),
	m_memidx	(1),
	m_memOverride	(0)
{}

//------------------------------------------------------------------------------
VEDAhmemptr Context::hmemAlloc(const size_t size) {
	VEDAhmemptr ptr;
	if(veda::internal::profiler::wrap(veo_alloc_hmem, m_handle, (void**)&ptr, size) != 0)
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
	// TODO: port to TUNGL
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
StreamGuard Context::stream(const VEDAstream stream) {
	VEDA_ASSERT((stream >= -1) && (stream < (VEDAstream)m_streams.size()), VEDA_ERROR_UNKNOWN_STREAM);
	return {m_streams[std::max(stream, 0)], stream < 0};
}

//------------------------------------------------------------------------------
// Kernels
//------------------------------------------------------------------------------
std::string_view Context::kernelName(const Kernel k) const {
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
		case VEDA_KERNEL_RAW_MEMCPY_D2D:	return "veda_raw_memcpy_d2d";
		case VEDA_KERNEL_RAW_MEMSET_U128:	return "veda_raw_memset_u128";
		case VEDA_KERNEL_RAW_MEMSET_U128_2D:	return "veda_raw_memset_u128_2d";
		case VEDA_KERNEL_RAW_MEMSET_U16:	return "veda_raw_memset_u16";
		case VEDA_KERNEL_RAW_MEMSET_U16_2D:	return "veda_raw_memset_u16_2d";
		case VEDA_KERNEL_RAW_MEMSET_U32:	return "veda_raw_memset_u32";
		case VEDA_KERNEL_RAW_MEMSET_U32_2D:	return "veda_raw_memset_u32_2d";
		case VEDA_KERNEL_RAW_MEMSET_U64:	return "veda_raw_memset_u64";
		case VEDA_KERNEL_RAW_MEMSET_U64_2D:	return "veda_raw_memset_u64_2d";
		case VEDA_KERNEL_RAW_MEMSET_U8:		return "veda_raw_memset_u8";
		case VEDA_KERNEL_RAW_MEMSET_U8_2D:	return "veda_raw_memset_u8_2d";
		case VEDA_KERNEL_MEM_PTR:		return "veda_mem_ptr";
		case VEDA_KERNEL_MEM_ASSIGN:		return "veda_mem_assign";
		case VEDA_KERNEL_MEM_REMOVE:		return "veda_mem_remove";
		case VEDA_KERNEL_MEM_SIZE:		return "veda_mem_size";
		case VEDA_KERNEL_MEM_SWAP:		return "veda_mem_swap";
	}

	VEDA_THROW(VEDA_ERROR_UNKNOWN_KERNEL);
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
veo_sym Context::moduleGetFunction(const Module* mod, const std::string_view name) {
	VEDA_ASSERT(name.size() && name.begin(), VEDA_ERROR_INVALID_VALUE);
	auto sym = veo_get_sym(m_handle, mod ? mod->lib() : 0, name.begin());
	VEDA_ASSERT(sym, VEDA_ERROR_FUNCTION_NOT_FOUND);
	return sym;
}

//------------------------------------------------------------------------------
Module* Context::moduleLoad(std::string name) {
	if(name.empty())
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	
	if(devices::architecture() == 3) {
		if(auto pos = name.find(".vso"); pos != std::string::npos) {
			std::string name_(name);
			name_.replace(pos, 4, ".vso3");

			struct stat buffer;
			if(!stat(name_.c_str(), &buffer) && S_ISREG(buffer.st_mode))
				std::swap(name, name_);
		}
	}

	auto lib = veo_load_library(m_handle, name.c_str());
	if(lib == 0)
		VEDA_THROW(VEDA_ERROR_MODULE_NOT_FOUND);
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
				// TODO: this issues many kernel calls when lazy allocation is used!
				auto vptr = VEDA_SET_PTR(device().vedaId(), idx, 0);
				auto s = stream(0);
				s.enqueue(false, Result(&info->ptr),  kernel(VEDA_KERNEL_MEM_PTR),  vptr);
				s.enqueue(false, Result(&info->size), kernel(VEDA_KERNEL_MEM_SIZE), vptr);
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
		if(info->size != size) {
			L_TRACE("[ve:%i] Cannot override pointer using %lluB, expected %lluB", device().vedaId(), size, info->size);
			VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
		}
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
		auto s	= stream(_stream);
		s.enqueue(veo_alloc_mem_async, false, {}, 0, size);
		s.enqueue(false, Result(&info->ptr), kernel(VEDA_KERNEL_MEM_ASSIGN), vptr, size);
	}

	return vptr;
}

//------------------------------------------------------------------------------
Context::VPtrTuple Context::memAllocPitch(const size_t w_bytes, const size_t h, const uint32_t elementSize, VEDAstream stream) {
	return std::make_tuple(memAlloc(w_bytes * h, stream), w_bytes);
}

//------------------------------------------------------------------------------
void Context::memSwap(VEDAdeviceptr A, VEDAdeviceptr B, VEDAstream _stream) {
	LOCK(mutex_ptrs);
	auto get = [this](VEDAdeviceptr x) {
		auto it = m_ptrs.find(VEDA_GET_IDX(x));
		if(it == m_ptrs.end())
			VEDA_THROW(VEDA_ERROR_UNKNOWN_VPTR);
		return it;
	};

	std::swap(get(A)->second, get(B)->second);
	stream(_stream).enqueue(true, {}, kernel(VEDA_KERNEL_MEM_SWAP), A, B);
}

//------------------------------------------------------------------------------
void Context::memFree(VEDAdeviceptr vptr, VEDAstream _stream) {
	ASSERT(VEDA_GET_DEVICE(vptr) == device().vedaId());

	// If this context is not active, we don't care about still pending frees.
	if(!isActive())
		return;

	VEDA_ASSERT(VEDA_GET_OFFSET(vptr) == 0, VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED);

	LOCK(mutex_ptrs);
	auto it = m_ptrs.find(VEDA_GET_IDX(vptr));
	VEDA_ASSERT(it != m_ptrs.end(), VEDA_ERROR_UNKNOWN_VPTR);
	
	auto info = it->second;
	
	/** This is a special case. When we issue AsyncMalloc and immediately
	 * do AsyncFree, then the data might not have arrived on the host yet
	 * causing a Segfault */
	if(info->ptr == 0 && info->size != 0)
		sync();

	if(info->size) {
		auto s = stream(_stream);
		s.enqueue(true, {}, kernel(VEDA_KERNEL_MEM_REMOVE), vptr);
		s.enqueue(veo_free_mem_async, false, {}, 0, (uint64_t)info->ptr);
	}

	m_ptrs.erase(it);
}

//------------------------------------------------------------------------------
VEDAdeviceptrInfo Context::getPtr(VEDAdeviceptr vptr) {
	ASSERT(VEDA_GET_DEVICE(vptr) == device().vedaId());
	
	LOCK(mutex_ptrs);
	auto it = m_ptrs.find(VEDA_GET_IDX(vptr));
	VEDA_ASSERT(it != m_ptrs.end(), VEDA_ERROR_UNKNOWN_VPTR);
	
	auto info = it->second;

	if(info->ptr == 0) syncPtrs();
	if(info->ptr == 0) return *info;

	return {((char*)info->ptr) + VEDA_GET_OFFSET(vptr), info->size};
}

//------------------------------------------------------------------------------
// Function Calls
//------------------------------------------------------------------------------
ReqId Context::call(VEDAfunction func, VEDAstream _stream, VEDAargs args, const bool destroyArgs, const bool checkResult, ResultPtr result) {
	auto id = stream(_stream).enqueue(veo_call_async, checkResult, result, func.kernel, func.ptr, args);
	if(destroyArgs)
		TVEDA(vedaArgsDestroy(args));
	return id;
}

//------------------------------------------------------------------------------
ReqId Context::call(VEDAhost_function func, VEDAstream _stream, void* userData, const bool checkResult, ResultPtr result) {
	return stream(_stream).enqueue(veo_call_async_vh, checkResult, result, 0, func, userData);
}

//------------------------------------------------------------------------------
// Memcpy
//------------------------------------------------------------------------------
void Context::memcpyD2D(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size, VEDAstream _stream) {
	if(!dst || !src)
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	stream(_stream).enqueue(true, {}, kernel(VEDA_KERNEL_MEMCPY_D2D), dst, src, size);
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

	stream(_stream).enqueue(veo_async_read_mem, false, {}, 0, dst, (veo_ptr)ptr, bytes);
}

//------------------------------------------------------------------------------
void Context::memcpyH2D(VEDAdeviceptr dst, const void* src, const size_t bytes, VEDAstream _stream) {
	VEDA_ASSERT(dst && src, VEDA_ERROR_INVALID_VALUE);

	auto res	= getPtr(dst);
	auto ptr	= res.ptr;	ASSERT(ptr);
	auto size	= res.size;	ASSERT(size);

	VEDA_ASSERT((bytes + VEDA_GET_OFFSET(dst)) <= size, VEDA_ERROR_OUT_OF_BOUNDS);

	stream(_stream).enqueue(veo_async_write_mem, false, {}, 0, (veo_ptr)ptr, src, bytes);
}

//------------------------------------------------------------------------------
void Context::sync(VEDAstream _stream, const ReqId until) {
	auto sync = _stream >= 0 ?
		(std::function<void(void)>)[this, _stream, until] {
			stream(_stream)->sync(until);
		} :
		(std::function<void(void)>)[this] {
			for(int i = 0; i < m_streams.size(); i++)
				stream(i)->sync();
		};

	if(auto data = profiler::data(device().vedaId(), _stream, false, VEDA_PROFILER_SYNC)) {
		profiler::callbackIssue(data);
		profiler::callbackBegin(data);
		sync();
		profiler::callbackEnd(data);
	} else {
		sync();
	}
}

//------------------------------------------------------------------------------
VEDAresult Context::query(VEDAstream _stream) {
	switch(stream(_stream)->state()) {
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
	VEDA_ASSERT(!isActive(), VEDA_ERROR_CANNOT_CREATE_CONTEXT);

	// Set Class Variables -------------------------------------------------
	m_mode = mode;

	// Create AVEO proc ----------------------------------------------------
	int numStreams	= 0;
	int cores	= device().cores();
	if(auto omp = veda::internal::ompThreads())
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
	VEDA_ASSERT(m_handle, VEDA_ERROR_CANNOT_CREATE_CONTEXT);

	// Load STDLib ---------------------------------------------------------
	// Load the VEDA kernel module on the VE device memory.
	m_lib = moduleLoad(veda::internal::stdLib());
	for(int i = 0; i < VEDA_KERNEL_CNT; i++)
		m_kernels[i] = m_lib->getFunction(kernelName((Kernel)i));

	// Create Streams ------------------------------------------------------
	auto device_id = this->device().vedaId();
	m_streams.reserve(numStreams);
	for(int stream_id = 0; stream_id < numStreams; stream_id++)
		m_streams.emplace_back(veo_context_open(m_handle), device_id, stream_id);

	// Fetch Proc ID -------------------------------------------------------
	m_aveoProcId = veo_proc_identifier(m_handle);

	devices::map(m_aveoProcId, device());
}

//------------------------------------------------------------------------------
void Context::destroy(void) {
	VEDA_ASSERT(isActive(), VEDA_ERROR_CONTEXT_IS_DESTROYED);

	LOCK(mutex_ptrs);
	syncPtrs();

	if(veda::internal::isMemTrace()) {
		for(auto&& [idx, info] : m_ptrs) {
			auto vptr = (VEDAdeviceptr)(VEDA_SET_PTR(device().vedaId(), idx, 0));
			printf("[VEDA ERROR]: VEDAdeviceptr %p with size %lluB has not been freed!\n", vptr, info->size);
		}
	}
	
	if(m_handle) {
		devices::unmap(m_aveoProcId);
		TVEO(veo_proc_destroy(m_handle));
		m_handle	= 0;
		m_aveoProcId	= 0;
	}

	m_streams.clear();	// don't need to be destroyed
	m_modules.clear();	// don't need to be destroyed
	m_kernels.clear();	// don't need to be destroyed
	m_ptrs.clear();		// no need to destroy info
	m_lib		= 0;
	m_mode		= VEDA_CONTEXT_MODE_OMP;
	m_memidx	= 0;
}

//------------------------------------------------------------------------------
// Memset
//------------------------------------------------------------------------------
#define VEDA_MEMSET_KERNEL(d, t, k) \
template<typename D, typename T>\
inline typename std::enable_if<std::is_same<D, d>::value && (sizeof(T)) == t, Kernel>::type memset_kernel(void) {\
	return k;\
}

VEDA_MEMSET_KERNEL(VEDAdeviceptr, 1, VEDA_KERNEL_MEMSET_U8)
VEDA_MEMSET_KERNEL(VEDAdeviceptr, 2, VEDA_KERNEL_MEMSET_U16)
VEDA_MEMSET_KERNEL(VEDAdeviceptr, 4, VEDA_KERNEL_MEMSET_U32)
VEDA_MEMSET_KERNEL(VEDAdeviceptr, 8, VEDA_KERNEL_MEMSET_U64)
VEDA_MEMSET_KERNEL(VEDAhmemptr, 1, VEDA_KERNEL_RAW_MEMSET_U8)
VEDA_MEMSET_KERNEL(VEDAhmemptr, 2, VEDA_KERNEL_RAW_MEMSET_U16)
VEDA_MEMSET_KERNEL(VEDAhmemptr, 4, VEDA_KERNEL_RAW_MEMSET_U32)
VEDA_MEMSET_KERNEL(VEDAhmemptr, 8, VEDA_KERNEL_RAW_MEMSET_U64)

#undef VEDA_MEMSET_KERNEL

//------------------------------------------------------------------------------
#define VEDA_MEMSET_KERNEL(d, t, k)\
template<typename D, typename T>\
inline typename std::enable_if<std::is_same<D, d>::value && (sizeof(T)) == t, Kernel>::type memset2d_kernel(void) {\
	return k;\
}

VEDA_MEMSET_KERNEL(VEDAdeviceptr, 1, VEDA_KERNEL_MEMSET_U8_2D)
VEDA_MEMSET_KERNEL(VEDAdeviceptr, 2, VEDA_KERNEL_MEMSET_U16_2D)
VEDA_MEMSET_KERNEL(VEDAdeviceptr, 4, VEDA_KERNEL_MEMSET_U32_2D)
VEDA_MEMSET_KERNEL(VEDAdeviceptr, 8, VEDA_KERNEL_MEMSET_U64_2D)
VEDA_MEMSET_KERNEL(VEDAhmemptr, 1, VEDA_KERNEL_RAW_MEMSET_U8_2D)
VEDA_MEMSET_KERNEL(VEDAhmemptr, 2, VEDA_KERNEL_RAW_MEMSET_U16_2D)
VEDA_MEMSET_KERNEL(VEDAhmemptr, 4, VEDA_KERNEL_RAW_MEMSET_U32_2D)
VEDA_MEMSET_KERNEL(VEDAhmemptr, 8, VEDA_KERNEL_RAW_MEMSET_U64_2D)
#undef VEDA_MEMSET_KERNEL

//------------------------------------------------------------------------------
template<typename D, typename T>
void Context::memset(D dst, const T value, const size_t cnt, VEDAstream _stream) {
	stream(_stream).enqueue(true, {}, kernel(memset_kernel<D, T>()), dst, value, cnt);
}

template void Context::memset<VEDAdeviceptr, uint8_t> (VEDAdeviceptr, const uint8_t,  const size_t, VEDAstream);
template void Context::memset<VEDAdeviceptr, uint16_t>(VEDAdeviceptr, const uint16_t, const size_t, VEDAstream);
template void Context::memset<VEDAdeviceptr, uint32_t>(VEDAdeviceptr, const uint32_t, const size_t, VEDAstream);
template void Context::memset<VEDAdeviceptr, uint64_t>(VEDAdeviceptr, const uint64_t, const size_t, VEDAstream);
template void Context::memset<VEDAhmemptr,   uint8_t> (VEDAhmemptr,   const uint8_t,  const size_t, VEDAstream);
template void Context::memset<VEDAhmemptr,   uint16_t>(VEDAhmemptr,   const uint16_t, const size_t, VEDAstream);
template void Context::memset<VEDAhmemptr,   uint32_t>(VEDAhmemptr,   const uint32_t, const size_t, VEDAstream);
template void Context::memset<VEDAhmemptr,   uint64_t>(VEDAhmemptr,   const uint64_t, const size_t, VEDAstream);

//------------------------------------------------------------------------------
// Memset128
//------------------------------------------------------------------------------
template<typename D>
void Context::memset(D dst, const uint64_t x, const uint64_t y, const size_t cnt, VEDAstream _stream) {
	stream(_stream).enqueue(true, {}, kernel(std::is_same<D, VEDAdeviceptr>::value ? VEDA_KERNEL_MEMSET_U128 : VEDA_KERNEL_RAW_MEMSET_U128), dst, x, y, cnt);
}

template void Context::memset<VEDAdeviceptr>(VEDAdeviceptr, const uint64_t, const uint64_t, const size_t, VEDAstream);
template void Context::memset<VEDAhmemptr>  (VEDAhmemptr,   const uint64_t, const uint64_t, const size_t, VEDAstream);

//------------------------------------------------------------------------------
// Memset2D
//------------------------------------------------------------------------------
template<typename D, typename T>
void Context::memset2D(D dst, const size_t pitch, const T value, const size_t w, const size_t h, VEDAstream _stream) {
	stream(_stream).enqueue(true, {}, kernel(memset2d_kernel<D, T>()), dst, pitch, value, w, h);
}

template void Context::memset2D<VEDAdeviceptr, uint8_t> (VEDAdeviceptr, const size_t, const uint8_t,  const size_t, const size_t, VEDAstream);
template void Context::memset2D<VEDAdeviceptr, uint16_t>(VEDAdeviceptr, const size_t, const uint16_t, const size_t, const size_t, VEDAstream);
template void Context::memset2D<VEDAdeviceptr, uint32_t>(VEDAdeviceptr, const size_t, const uint32_t, const size_t, const size_t, VEDAstream);
template void Context::memset2D<VEDAdeviceptr, uint64_t>(VEDAdeviceptr, const size_t, const uint64_t, const size_t, const size_t, VEDAstream);
template void Context::memset2D<VEDAhmemptr,   uint8_t> (VEDAhmemptr,   const size_t, const uint8_t,  const size_t, const size_t, VEDAstream);
template void Context::memset2D<VEDAhmemptr,   uint16_t>(VEDAhmemptr,   const size_t, const uint16_t, const size_t, const size_t, VEDAstream);
template void Context::memset2D<VEDAhmemptr,   uint32_t>(VEDAhmemptr,   const size_t, const uint32_t, const size_t, const size_t, VEDAstream);
template void Context::memset2D<VEDAhmemptr,   uint64_t>(VEDAhmemptr,   const size_t, const uint64_t, const size_t, const size_t, VEDAstream);

//------------------------------------------------------------------------------
// Memset2D 128
//------------------------------------------------------------------------------
template<typename D>
void Context::memset2D(D dst, const size_t pitch, const uint64_t x, const uint64_t y, const size_t w, const size_t h, VEDAstream _stream) {
	stream(_stream).enqueue(true, {}, kernel(std::is_same<D, VEDAdeviceptr>::value ? VEDA_KERNEL_MEMSET_U128_2D : VEDA_KERNEL_RAW_MEMSET_U128_2D), dst, pitch, x, y, w, h);
}

template void Context::memset2D<VEDAdeviceptr>(VEDAdeviceptr, const size_t, const uint64_t, const uint64_t, const size_t, const size_t, VEDAstream);
template void Context::memset2D<VEDAhmemptr>  (VEDAhmemptr,   const size_t, const uint64_t, const uint64_t, const size_t, const size_t, VEDAstream);

//------------------------------------------------------------------------------
	}
}
