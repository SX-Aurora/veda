#pragma once

//------------------------------------------------------------------------------
struct __VEDAcontext final {
private:
	VEDAproc											m_proc;
	veo_thr_ctxt*										m_ctx;
	std::vector<std::tuple<uint64_t, VEDAargs, bool>>	m_calls;
	std::list<__VEDAstream>								m_streams;
	std::mutex											m_mutex;


public:
					__VEDAcontext	(VEDAproc proc, veo_thr_ctxt* ctx);
	virtual			~__VEDAcontext	(void);
	VEDAproc		proc			(void) const;
	VEDAresult		getPtr			(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr);
	VEDAresult		call			(VEDAfunction func, VEDAargs args, const bool destroyArgs, const bool checkResult = false);
	VEDAresult		call			(VEDAhost_function func, void* userData);
	VEDAresult		memAlloc		(VEDAdeviceptr* vptr, const size_t size);
	VEDAresult		memAllocPitch	(VEDAdeviceptr* vptr, size_t* pitch, const size_t w_bytes, const size_t h, const uint32_t elementSize);
	VEDAresult		memFree			(VEDAdeviceptr vptr);
	VEDAresult		memPtr			(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr);
	VEDAresult		memcpyD2D		(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size);
	VEDAresult		memcpyD2H		(void* dst, VEDAdeviceptr src, const size_t size);
	VEDAresult		memcpyH2D		(VEDAdeviceptr dst, const void* src, const size_t size);
	VEDAresult		memset			(VEDAdeviceptr dst, const uint16_t value, const size_t size);
	VEDAresult		memset			(VEDAdeviceptr dst, const uint32_t value, const size_t size);
	VEDAresult		memset			(VEDAdeviceptr dst, const uint64_t value, const size_t size);
	VEDAresult		memset			(VEDAdeviceptr dst, const uint8_t value, const size_t size);
	VEDAresult		memset2D		(VEDAdeviceptr dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h);
	VEDAresult		memset2D		(VEDAdeviceptr dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h);
	VEDAresult		memset2D		(VEDAdeviceptr dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h);
	VEDAresult		memset2D		(VEDAdeviceptr dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h);
	VEDAresult		query			(void);
	VEDAresult		streamCreate	(VEDAstream* stream);
	VEDAresult		streamDestroy	(VEDAstream stream);
	VEDAresult		sync			(void);
	veo_thr_ctxt*	ctx				(void) const;
};

//------------------------------------------------------------------------------