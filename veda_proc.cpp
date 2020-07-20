#include "veda_internal.h"

#define LOCK() std::lock_guard<std::mutex> lock(veda_procs_mutex)

//------------------------------------------------------------------------------
static std::map<VEDAdevice, __VEDAproc> veda_procs;
static std::mutex veda_procs_mutex;

//------------------------------------------------------------------------------
VEDAresult vedaProcExit(void) {
	LOCK();
	for(auto& it : veda_procs)
		CVEDA(it.second.destroy());	
	veda_procs.clear();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaProcMemReport(void) {
	LOCK();
	for(auto& it : veda_procs)
		CVEDA(it.second.memReport());
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaProcDestroy(VEDAproc proc) {
	LOCK();
	CVEDA(proc->destroy());
	veda_procs.erase(proc->device());
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaProcGet(VEDAproc* proc, const VEDAdevice device) {
	LOCK();
	auto it = veda_procs.find(device);
	if(it != veda_procs.end())	*proc = &it->second;
	else						*proc = 0;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaProcCreate(VEDAproc* proc, const VEDAdevice device) {
	LOCK();
	int cnt;
	CVEDA(vedaDeviceGetCount(&cnt));
	if(device < 0 || device >= cnt)
		return VEDA_ERROR_INVALID_DEVICE;
	
	auto it = veda_procs.find(device);
	if(it == veda_procs.end()) {
		int idx = 0;
		CVEDA(vedaDeviceGetPhysicalIdx(&idx, device));
		auto ptr = veo_proc_create(idx);
		if(ptr == 0)	return VEDA_ERROR_CANNOT_CREATE_DEVICE;
		*proc = &veda_procs.emplace(MAP_EMPLACE(device, device, ptr)).first->second;
		CVEDA((*proc)->init());
	} else {
		*proc = &it->second;
	}
	
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaProcGetCurrent(VEDAproc* proc) {
	VEDAcontext ctx = 0;
	CVEDA(vedaCtxGetCurrent(&ctx));
	*proc = ctx->proc();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------