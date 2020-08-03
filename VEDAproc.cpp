#include "veda_internal.h"

#define LOCK() std::lock_guard<std::mutex> lock(m_mutex)

//------------------------------------------------------------------------------
__VEDAproc::__VEDAproc(VEDAdevice device, veo_proc_handle* handle) :
	m_kernels	(VEDA_KERNEL_CNT),
	m_device	(device),
	m_handle	(handle),
	m_lib		(0),
	m_memidx	(1)
{}

//------------------------------------------------------------------------------
__VEDAproc::~__VEDAproc(void) {
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::memReport(void) {
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
VEDAdevice __VEDAproc::device(void) const {
	return m_device;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::destroy(void) {
	CVEDA(syncPtrs());
	for(auto& it : m_ptrs) {
		auto idx	= it.first;
		auto& size	= std::get<1>(it.second);

		VEDAptr vptr(device(), idx);
		printf("[VEDA ERROR]: VEDAdeviceptr %p with size %lluB has not been freed!\n", vptr.vptr(), size);
	}

	CVEO(veo_proc_destroy(m_handle));
	m_ctxs.clear();		// don't need to be destroyed
	m_modules.clear();	// don't need to be destroyed
	m_kernels.clear();	// don't need to be destroyed
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::init(void) {
	assert(m_lib == 0);
	assert(m_kernels.size() == VEDA_KERNEL_CNT);

	CVEDA(moduleLoad(&m_lib, "libveda.vso"));
	for(int i = 0; i < VEDA_KERNEL_CNT; i++) {
		const char* name = 0;
		CVEDA(vedaKernelGetName(&name, (VEDAkernel)i));
		CVEDA(moduleGetFunction(&m_kernels[i], m_lib, name));
	}

	auto vctx = veo_context_open(m_handle);
	if(vctx == 0)	return VEDA_ERROR_CANNOT_CREATE_CONTEXT;
	m_ctxs.emplace(MAP_EMPLACE(vctx, this, vctx));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
const char* __VEDAproc::kernelName(VEDAfunction func) const {
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
VEDAfunction __VEDAproc::kernel(VEDAkernel kernel) const {
	if(kernel < 0 || kernel >= VEDA_KERNEL_CNT)
		return VEDA_ERROR_UNKNOWN_KERNEL;
	return m_kernels[kernel];
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::moduleGetFunction(VEDAfunction* func, VEDAmodule mod, const char* name) {
	if(name == 0)	return VEDA_ERROR_INVALID_VALUE;
	LOCK();
	*func =  veo_get_sym(m_handle, mod ? mod->lib() : 0, name);
	return *func == 0 ? VEDA_ERROR_FUNCTION_NOT_FOUND : VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::moduleLoad(VEDAmodule* mod, const char* name) {
	if(name == 0)	return VEDA_ERROR_INVALID_VALUE;
	LOCK();
	auto lib = veo_load_library(m_handle, name);
	if(lib == 0)	return VEDA_ERROR_MODULE_NOT_FOUND;
	*mod = &m_modules.emplace(MAP_EMPLACE(lib, this, lib)).first->second;
	(*mod)->inc();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::moduleUnload(VEDAmodule mod) {
	LOCK();
	if(mod->dec() == 0) {
		CVEO(veo_unload_library(m_handle, mod->lib()));
		m_modules.erase(mod->lib());
	}
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::ctxCreate(VEDAcontext* ctx) {
	*ctx = ctxDefault();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAcontext __VEDAproc::ctxDefault(void) {
	assert(m_ctxs.size() == 1);
	return &m_ctxs.begin()->second;
}

//------------------------------------------------------------------------------
size_t __VEDAproc::ctxCount(void) const {
	return m_ctxs.size();
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::ctxDestroy(VEDAcontext ctx) {
	/*LOCK();
	CVEO(veo_context_close(ctx->ctx()));
	m_ctxs.erase(ctx->ctx());*/
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
void __VEDAproc::incMemIdx(void) {
	m_memidx++;
	m_memidx = m_memidx & 0x00FFFFFF;
	if(m_memidx == 0)
		m_memidx = 1;
}

//------------------------------------------------------------------------------
VEDAdeviceptr __VEDAproc::newVPTR(veo_ptr** ptr, const size_t size) {
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
VEDAresult __VEDAproc::free(VEDAdeviceptr vptr) {
	VEDAptr pptr(vptr);
	assert(pptr.device() == device());
	return m_ptrs.erase(pptr.idx()) == 0 ? VEDA_ERROR_UNKNOWN_VPTR : VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::getBasePtr(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr) {
	*size = 0;
	*ptr = 0;

	VEDAptr pptr(vptr);
	assert(pptr.device() == device());
	auto it = m_ptrs.find(pptr.idx());
	if(it == m_ptrs.end()) {
		TRACE("VEDAproc::getBasePtr(%p, %p, %p)\n", *ptr, *size, vptr);
		return VEDA_ERROR_UNKNOWN_VPTR;
	}
	
	if(std::get<0>(it->second) == 0)
		CVEDA(syncPtrs());

	*ptr	= std::get<0>(it->second);
	*size	= std::get<1>(it->second);
	TRACE("VEDAproc::getBasePtr(%p, %p, %p)\n", *ptr, *size, vptr);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::getFreeMem(size_t* free, const size_t total) {
	CVEDA(syncPtrs());

	size_t used = 0;
	for(auto& it : m_ptrs)
		used += std::get<1>(it.second);

	*free = total - used;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult __VEDAproc::syncPtrs(void) {
	TRACE("VEDAproc::syncPtrs()\n");
	auto ctx = ctxDefault();
	bool sync = false;

	for(auto& it : m_ptrs) {
		auto idx	= it.first;
		auto& ptr	= std::get<0>(it.second);
		auto& size	= std::get<1>(it.second);

		if(ptr == 0) {
			// if size is == 0, then no malloc call had been issued, so we need to fetch the info
			// is size is != 0, then we only need to wait till the malloc reports back the ptr
			if(size == 0) {
				VEDAptr vptr(device(), idx);
				CVEDA(ctx->memPtr(&ptr, &size, vptr.vptr()));
			}
			sync = true;
		}
	}

	if(sync)
		CVEDA(ctx->sync());
	
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------