#include "veda_internal.h"

//------------------------------------------------------------------------------
#define LOCK() std::lock_guard<std::mutex> lock(veda_ctxs_mutex)

thread_local std::list<VEDAcontext> veda_thread_ctxs;
static std::map<VEDAdevice, __VEDAcontext> veda_ctxs;
static std::mutex veda_ctxs_mutex;

//------------------------------------------------------------------------------
VEDAresult vedaCtxExit(void) {
	LOCK();
	for(auto& it : veda_ctxs)
		CVEDA(it.second.destroy());	
	veda_ctxs.clear();

	veda_thread_ctxs.clear(); // this is kind of unsafe, as not all threads get cleared...
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxMemReport(void) {
	LOCK();
	for(auto& it : veda_ctxs)
		CVEDA(it.second.memReport());
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaCtxGet(VEDAcontext* ctx, const VEDAdevice device) {
	CVEDA(vedaIsDevice(device));
	
	LOCK();
	auto it = veda_ctxs.find(device);
	if(it != veda_ctxs.end())	*ctx = &it->second;
	else						*ctx = 0;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxCreate(VEDAcontext* pctx, int mode, VEDAdevice device) {
	CVEDA(vedaCtxGet(pctx, device)); // Check if a context already exists
	if(*pctx != 0)
		return VEDA_ERROR_CANNOT_CREATE_CONTEXT;

	int cores;
	CVEDA(vedaDeviceGetAttribute(&cores, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, device));
	cores = std::min(cores, vedaOmpThreads());

	LOCK();
	int numStreams;
	if(mode == VEDA_CONTEXT_MODE_OMP) {
		char buffer[3];
		snprintf(buffer, sizeof(buffer), "%i", cores);
		setenv("VE_OMP_NUM_THREADS", buffer, 1);
		numStreams = 1;
	} else if(mode == VEDA_CONTEXT_MODE_SCALAR) {
		setenv("VE_OMP_NUM_THREADS", "1", 1);
		numStreams = cores;
	} else {
		return VEDA_ERROR_INVALID_VALUE;
	}

	int idx = 0;
	CVEDA(vedaDeviceGetPhysicalIdx(&idx, device));
	auto ptr = veo_proc_create(idx);
	if(ptr == 0)
		return VEDA_ERROR_CANNOT_CREATE_CONTEXT;
	*pctx = &veda_ctxs.emplace(MAP_EMPLACE(device, device, ptr, numStreams)).first->second;
	CVEDA((*pctx)->init());
	
	return vedaCtxPushCurrent(*pctx);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxDestroy(VEDAcontext ctx) {
	GUARDED(
		LOCK();
		CVEDA(ctx->destroy());
		veda_ctxs.erase(ctx->device());
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetApiVersion(VEDAcontext ctx, uint32_t* version) {
	GUARDED(
		uint64_t value = 0;
		CVEDA(vedaDeviceGetInfo(&value, "abi_version", ctx->device()));
		*version = (uint32_t)value;
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxStreamCnt(int* cnt) {
	GUARDED(
		VEDAcontext ctx;
		CVEDA(vedaCtxGetCurrent(&ctx));
		*cnt = (int)ctx->streamCount();
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetCurrent(VEDAcontext* pctx) {
	GUARDED(
		if(veda_thread_ctxs.empty())
			return VEDA_ERROR_NO_CONTEXT;
		*pctx = veda_thread_ctxs.back();
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetDevice(VEDAdevice* device) {
	GUARDED(
		VEDAcontext ctx;
		CVEDA(vedaCtxGetCurrent(&ctx));
		*device = ctx->device();
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxPopCurrent(VEDAcontext* pctx) {
	GUARDED(
		if(veda_thread_ctxs.empty())
			return VEDA_ERROR_CANNOT_POP_CONTEXT;
		*pctx = veda_thread_ctxs.back();
		veda_thread_ctxs.pop_back();
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxPushCurrent(VEDAcontext ctx) {
	GUARDED(
		veda_thread_ctxs.emplace_back(ctx);
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxSetCurrent(VEDAcontext ctx) {
	GUARDED(
		if(ctx) {
			if(veda_thread_ctxs.empty())	veda_thread_ctxs.emplace_back(ctx);
			else				veda_thread_ctxs.back() = ctx;
		} else {
			if(!veda_thread_ctxs.empty())	veda_thread_ctxs.pop_back();
		}
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxSynchronize(void) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->sync());
	);
}

//------------------------------------------------------------------------------
} // extern "C"