#include "veda/veda.hpp"

//------------------------------------------------------------------------------
extern "C" {
//------------------------------------------------------------------------------
VEDAresult	vedaCtxDestroy		(VEDAcontext ctx)			{	GUARDED(ctx->device().destroyCtx();)					}
VEDAresult	vedaCtxGetApiVersion	(VEDAcontext ctx, uint32_t* version)	{	GUARDED(*version = (uint32_t)ctx->device().versionAbi();)		}
VEDAresult	vedaCtxGetCurrent	(VEDAcontext* pctx)			{	GUARDED(*pctx = veda::Contexts::current();)				}
VEDAresult	vedaCtxGetDevice	(VEDAdevice* device)			{	GUARDED(*device = veda::Contexts::current()->device().vedaId();)	}
VEDAresult	vedaCtxPopCurrent	(VEDAcontext* pctx)			{	GUARDED(*pctx = veda::Contexts::pop();)					}
VEDAresult	vedaCtxPushCurrent	(VEDAcontext ctx)			{	GUARDED(veda::Contexts::push(ctx);)					}
VEDAresult	vedaCtxSetCurrent	(VEDAcontext ctx)			{	GUARDED(veda::Contexts::set(ctx);)					}
VEDAresult	vedaCtxStreamCnt	(int* cnt)				{	GUARDED(*cnt = veda::Contexts::current()->streamCount();)		}
VEDAresult	vedaCtxSynchronize	(void)					{	GUARDED(veda::Contexts::current()->sync();)				}

//------------------------------------------------------------------------------
VEDAresult vedaCtxCreate(VEDAcontext* pctx, int mode, VEDAdevice dev) {
	GUARDED(
		*pctx = veda::Devices::get(dev).createCtx((VEDAcontext_mode)mode);
		veda::Contexts::push(*pctx);
	)
}

//------------------------------------------------------------------------------
} // extern "C"