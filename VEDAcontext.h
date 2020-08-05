#pragma once

struct __VEDAcontext final {
private:
	struct Stream {
		veo_thr_ctxt*										ctx;
		std::vector<std::tuple<uint64_t, VEDAargs, bool>>	calls;
	};

	std::mutex											m_mutex;
	std::map<veo_lib, __VEDAmodule>						m_modules;
	std::map<VEDAptr::Idx, std::tuple<veo_ptr, size_t>>	m_ptrs;

	std::vector<VEDAfunction>							m_kernels;
	std::vector<Stream>									m_streams;
	const VEDAdevice									m_device;
	veo_proc_handle* const								m_handle;
	VEDAmodule											m_lib;
	VEDAptr::Idx										m_memidx;

	void			incMemIdx			(void);
	VEDAresult		syncPtrs			(void);
	VEDAdeviceptr	newVPTR				(veo_ptr** ptr, const size_t size);
	VEDAresult		free				(VEDAdeviceptr vptr);
	VEDAresult		getBasePtr			(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr);

public:
					__VEDAcontext		(VEDAdevice device, veo_proc_handle* handle, const int numStreams);
					~__VEDAcontext		(void);
	VEDAdevice		device				(void) const;
	VEDAfunction	kernel				(VEDAkernel kernel) const;
	VEDAresult		call				(VEDAfunction func, VEDAstream stream, VEDAargs args, const bool destroyArgs, const bool checkResult = false);
	VEDAresult		call				(VEDAhost_function func, void* userData, VEDAstream stream);
	VEDAresult		destroy				(void);
	VEDAresult		getFreeMem			(size_t* free, const size_t total);
	VEDAresult		getPtr				(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr);
	VEDAresult		init				(void);
	VEDAresult		memAlloc			(VEDAdeviceptr* vptr, const size_t size, VEDAstream stream);
	VEDAresult		memAllocPitch		(VEDAdeviceptr* vptr, size_t* pitch, const size_t w_bytes, const size_t h, const uint32_t elementSize, VEDAstream stream);
	VEDAresult		memFree				(VEDAdeviceptr vptr, VEDAstream stream);
	VEDAresult		memPtr				(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr, VEDAstream stream);
	VEDAresult		memReport			(void);
	VEDAresult		memcpyD2D			(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size, VEDAstream stream);
	VEDAresult		memcpyD2H			(void* dst, VEDAdeviceptr src, const size_t size, VEDAstream stream);
	VEDAresult		memcpyH2D			(VEDAdeviceptr dst, const void* src, const size_t size, VEDAstream stream);
	VEDAresult		memset				(VEDAdeviceptr dst, const uint16_t value, const size_t size, VEDAstream stream);
	VEDAresult		memset				(VEDAdeviceptr dst, const uint32_t value, const size_t size, VEDAstream stream);
	VEDAresult		memset				(VEDAdeviceptr dst, const uint64_t value, const size_t size, VEDAstream stream);
	VEDAresult		memset				(VEDAdeviceptr dst, const uint8_t value, const size_t size, VEDAstream stream);
	VEDAresult		memset2D			(VEDAdeviceptr dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h, VEDAstream stream);
	VEDAresult		memset2D			(VEDAdeviceptr dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h, VEDAstream stream);
	VEDAresult		memset2D			(VEDAdeviceptr dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h, VEDAstream stream);
	VEDAresult		memset2D			(VEDAdeviceptr dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h, VEDAstream stream);
	VEDAresult		moduleGetFunction	(VEDAfunction* func, VEDAmodule mod, const char* name);
	VEDAresult		moduleLoad			(VEDAmodule* mod, const char* name);
	VEDAresult		moduleUnload		(VEDAmodule mod);
	VEDAresult		query				(VEDAstream stream);
	VEDAresult		stream				(veo_thr_ctxt** veo, const VEDAstream stream);
	VEDAresult		streamCreate		(VEDAstream* stream);
	VEDAresult		streamDestroy		(VEDAstream stream);
	VEDAresult		sync				(VEDAstream stream);
	VEDAresult		sync				(void);
	const char*		kernelName			(VEDAfunction func) const;
	size_t			streamCount			(void) const;
};