#include "veda/veda.hpp"

//------------------------------------------------------------------------------
namespace veda {
	thread_local std::list<VEDAcontext> s_thread_ctxs;
	static std::map<VEDAdevice, __VEDAcontext> s_ctxs;
	static std::mutex s_ctxs_mutex;
}

//------------------------------------------------------------------------------
extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaCtxGet(VEDAcontext* ctx, const VEDAdevice device) {
	GUARDED(*ctx = veda::Devices::get(device).ctx();)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxCreate(VEDAcontext* pctx, int mode, VEDAdevice dev) {
	GUARDED(
		auto& device = veda::Devices::get(dev);
		if(device.ctx())
			return VEDA_ERROR_CANNOT_CREATE_CONTEXT;
		*pctx = device.createCtx((VEDAcontext_mode)mode);
		veda::Contexts::push(*pctx);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxDestroy(VEDAcontext ctx) {
	GUARDED(delete ctx;)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetApiVersion(VEDAcontext ctx, uint32_t* version) {
	GUARDED(*version = (uint32_t)ctx->device().versionAbi();)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxStreamCnt(int* cnt) {
	GUARDED(*cnt = (int)veda::Contexts::current()->streamCount();)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetCurrent(VEDAcontext* pctx) {
	GUARDED(*pctx = veda::Contexts::current();)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxGetDevice(VEDAdevice* device) {
	GUARDED(*device = veda::Contexts::current()->device().vedaId();)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxPopCurrent(VEDAcontext* pctx) {
	GUARDED(
		if(veda::s_thread_ctxs.empty())
			return VEDA_ERROR_CANNOT_POP_CONTEXT;
		*pctx = veda::s_thread_ctxs.back();
		veda::s_thread_ctxs.pop_back();
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxPushCurrent(VEDAcontext ctx) {
	GUARDED(
		veda::s_thread_ctxs.emplace_back(ctx);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxSetCurrent(VEDAcontext ctx) {
	GUARDED(
		if(ctx) {
			if(veda::s_thread_ctxs.empty())		veda::s_thread_ctxs.emplace_back(ctx);
			else					veda::s_thread_ctxs.back() = ctx;
		} else {
			if(!veda::s_thread_ctxs.empty())	veda::s_thread_ctxs.pop_back();
		}
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaCtxSynchronize(void) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		ctx->sync();
	)
}

//------------------------------------------------------------------------------
} // extern "C"