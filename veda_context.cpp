#include "veda_internal.h"

//------------------------------------------------------------------------------
thread_local std::list<VEDAcontext> t_ctxs;

//------------------------------------------------------------------------------
VEDAresult vedaCtxExit(void) {
	t_ctxs.clear(); // this is kind of unsafe, as not all threads get cleared...
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaCtxCreate(VEDAcontext* pctx, uint32_t flags, VEDAdevice dev) {
	GUARDED(
		CVEDA(vedaIsDevice(dev));
		if(flags != 0)	return VEDA_ERROR_INVALID_VALUE;

		VEDAproc proc;
		CVEDA(vedaProcGet(&proc, dev));
		if(!proc)
			CVEDA(vedaProcCreate(&proc, dev));
		CVEDA(proc->ctxCreate(pctx));
		t_ctxs.emplace_back(*pctx);
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxDestroy(VEDAcontext ctx) {
	GUARDED(
		CVEDA(vedaIsContext(ctx));

		// Destroy Context
		auto proc = ctx->proc();
		CVEDA(proc->ctxDestroy(ctx));
		if(t_ctxs.back() == ctx)
			t_ctxs.pop_back();

		// If no further contexts, destroy proc
		if(proc->ctxCount() == 0)
			CVEDA(vedaProcDestroy(proc));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetApiVersion(VEDAcontext ctx, uint32_t* version) {
	GUARDED(
		uint64_t value = 0;
		CVEDA(vedaDeviceGetInfo(&value, "abi_version", ctx->proc()->device()));
		*version = (uint32_t)value;
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetCurrent(VEDAcontext* pctx) {
	GUARDED(
		if(t_ctxs.empty())	return VEDA_ERROR_NO_CONTEXT;
		*pctx = t_ctxs.back();
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetDevice(VEDAdevice* device) {
	GUARDED(
		VEDAproc proc;
		CVEDA(vedaProcGetCurrent(&proc));
		*device = proc->device();
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxPopCurrent(VEDAcontext* pctx) {
	GUARDED(
		if(t_ctxs.empty())	return VEDA_ERROR_CANNOT_POP_CONTEXT;
		*pctx = t_ctxs.back();
		t_ctxs.pop_back();
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxPushCurrent(VEDAcontext ctx) {
	GUARDED(
		t_ctxs.emplace_back(ctx);
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxSetCurrent(VEDAcontext ctx) {
	GUARDED(
		if(ctx) {
			if(t_ctxs.empty())	t_ctxs.emplace_back(ctx);
			else				t_ctxs.back() = ctx;
		} else {
			if(!t_ctxs.empty())	t_ctxs.pop_back();
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