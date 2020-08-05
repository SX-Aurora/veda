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
VEDAresult vedaCtxCreate(VEDAcontext* pctx, uint32_t flags, VEDAdevice device) {
	CVEDA(vedaCtxGet(pctx, device)); // Check if a context already exists
	if(*pctx != 0)
		return VEDA_ERROR_CANNOT_CREATE_CONTEXT;

	int cores;
	CVEDA(vedaDeviceGetAttribute(&cores, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, device));
	if(flags < 0 || flags > cores)
		return VEDA_ERROR_INVALID_VALUE;

	int omp = 0;

	if(!flags) {
		omp = vedaOmpThreads();
		if(!omp)
			omp = cores;
	} else {
		omp = flags;
	}

	if(omp == 0)
		return VEDA_ERROR_INVALID_VALUE;

	char buffer[3];
	snprintf(buffer, sizeof(buffer), "%i", omp);
	setenv("VE_OMP_NUM_THREADS", buffer, 1);

	int numStreams = cores/omp;
	if(numStreams == 0)
		return VEDA_ERROR_INVALID_VALUE;

	LOCK();

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
VEDAresult vedaCtxGetMaxStreams(int* streams) {
	GUARDED(
		VEDAcontext ctx;
		CVEDA(vedaCtxGetCurrent(&ctx));
		*streams = (int)ctx->streamCount();
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