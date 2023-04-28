#include <veda/internal/api.h>

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
void hmemcpy(VEDAhmemptr dst, VEDAhmemptr src, const size_t size) {
	if(profiler::wrap(veo_hmemcpy, dst, src, size) != 0)
		VEDA_THROW(VEDA_ERROR_UNKNOWN_HMEM);
}

//------------------------------------------------------------------------------
void hmemfree(VEDAhmemptr hmem) {
	if(profiler::wrap(veo_free_hmem, hmem) != 0)
		VEDA_THROW(VEDA_ERROR_UNKNOWN_HMEM);
}

//------------------------------------------------------------------------------
uint64_t memcpy_helper(void* data) {
	free(data);
	return 0;
}

//------------------------------------------------------------------------------
void memcpy(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size, const VEDAstream stream) {
	auto& D = devices::get(dst); auto& Dctx = D.ctx();
	auto& S = devices::get(src); auto& Sctx = S.ctx();

	if(D.vedaId() == S.vedaId()) {
		Dctx.memcpyD2D(dst, src, size, stream);
	} else {
		void* host = malloc(size);
		if(!host)
			VEDA_THROW(VEDA_ERROR_OUT_OF_MEMORY);
			
		Sctx.memcpyD2H(host, src, size, -1);
		Dctx.memcpyH2D(dst, host, size, stream);
		Dctx.call(memcpy_helper, stream, host, false, 0);
	}
}

//------------------------------------------------------------------------------
	}
}