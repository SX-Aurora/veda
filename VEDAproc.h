#pragma once

struct __VEDAproc final {
private:
	std::map<veo_lib, __VEDAmodule>						m_modules;
	std::map<veo_thr_ctxt*, __VEDAcontext>				m_ctxs;
	std::vector<VEDAfunction>							m_kernels;
	std::mutex											m_mutex;
	std::map<VEDAptr::Idx, std::tuple<veo_ptr, size_t>>	m_ptrs;

	const VEDAdevice									m_device;
	veo_proc_handle* const								m_handle;
	VEDAmodule											m_lib;
	VEDAptr::Idx										m_memidx;

	void			incMemIdx			(void);

public:
					__VEDAproc			(VEDAdevice device, veo_proc_handle* handle);
	virtual			~__VEDAproc			(void);
	VEDAcontext		ctxDefault			(void);
	VEDAdevice		device				(void) const;
	VEDAdeviceptr	newVPTR				(veo_ptr** ptr, const size_t size);
	VEDAfunction	kernel				(VEDAkernel kernel) const;
	VEDAresult		ctxCreate			(VEDAcontext* ctx);
	VEDAresult		ctxDestroy			(VEDAcontext ctx);
	VEDAresult		destroy				(void);
	VEDAresult		free				(VEDAdeviceptr vptr);
	VEDAresult		getBasePtr			(veo_ptr* ptr, size_t* size, VEDAdeviceptr vptr);
	VEDAresult		getFreeMem			(size_t* free, const size_t total);
	VEDAresult		init				(void);
	VEDAresult		moduleGetFunction	(VEDAfunction* func, VEDAmodule mod, const char* name);
	VEDAresult		moduleLoad			(VEDAmodule* mod, const char* name);
	VEDAresult		moduleUnload		(VEDAmodule mod);
	VEDAresult		syncPtrs			(const bool fullSync);
	VEDAresult		memReport			(void);
	size_t			ctxCount			(void) const;
};

typedef __VEDAproc* VEDAproc;