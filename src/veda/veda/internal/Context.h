#pragma once

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
class Context final {
public:
	using VPtrTuple = std::tuple<VEDAdeviceptr, size_t>;

private:
	using Ptrs	= std::unordered_map	<VEDAidx, VEDAdeviceptrInfo>;
	using Kernels	= std::vector		<VEDAfunction>;
	using Streams	= std::vector		<Stream>;
	using Modules	= std::map		<veo_lib, Module>;

		std::mutex		mutex_streams;
		std::mutex		mutex_ptrs;
		std::mutex		mutex_modules;

		VEDAcontext_mode	m_mode;
		Modules			m_modules;
		Ptrs			m_ptrs;
		Kernels			m_kernels;
		Streams			m_streams;
		Device&			m_device;
		veo_proc_handle*	m_handle;
		int			m_aveoProcId;
		VEDAmodule		m_lib;
		VEDAidx			m_memidx;
		VEDAdeviceptr		m_memOverride;
		ReqId			m_memLastAveoRequest;
		bool			m_memDelayedDirty;

	void			incMemIdx		(void);
	void			syncPtrs		(VEDAstream stream);

public:
				Context			(Device& device);
				Context			(const Context&) = delete;
	Device&			device			(void);
	Module*			moduleLoad		(std::string name);
	ReqId			call			(VEDAfunction func, VEDAstream stream, VEDAargs args, const bool destroyArgs, const bool checkResult, ResultPtr result);
	ReqId			call			(VEDAhost_function func, VEDAstream stream, void* userData, const bool checkResult, ResultPtr result);
	StreamGuard		stream			(const VEDAstream stream);
	VEDAcontext_mode	mode			(void) const;
	VEDAdeviceptr		memAlloc		(const size_t size, VEDAstream stream);
	VEDAdeviceptrInfo	getPtr			(VEDAdeviceptr vptr, VEDAstream stream = 0);
	VEDAfunction		kernel			(Kernel kernel) const;
	VEDAhmemptr		hmemAlloc		(const size_t size);
	VEDAresult		query			(VEDAstream stream);
	VPtrTuple		memAllocPitch		(const size_t w_bytes, const size_t h, const uint32_t elementSize, VEDAstream stream);
	bool			isActive		(void) const;
	int			aveoProcId		(void) const;
	int			streamCnt		(void) const;
	size_t			memUsed			(void);
	std::string_view	kernelName		(const Kernel k) const;
	veo_sym			moduleGetFunction	(const Module* mod, const std::string_view name);
	void			destroy			(void);
	void			init			(const VEDAcontext_mode mode);
	void			memRelease		(VEDAdeviceptr vptr);
	void			memFree			(VEDAdeviceptr vptr, VEDAstream stream);
	void			memReport		(void);
	void			memSwap			(VEDAdeviceptr A, VEDAdeviceptr B, VEDAstream stream);
	void			memcpyD2D		(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size, VEDAstream stream);
	void			memcpyD2H		(void* dst, VEDAdeviceptr src, const size_t size, VEDAstream stream);
	void			memcpyH2D		(VEDAdeviceptr dst, const void* src, const size_t size, VEDAstream stream);
	void			moduleUnload		(const Module* mod);
	void			setMemOverride		(VEDAdeviceptr vptr);
	void			sync			(VEDAstream stream, const ReqId until = -1);
	void			sync			(void);

template<typename D, typename T>	void memset	(D dst, const T value, const size_t size, VEDAstream stream);
template<typename D, typename T>	void memset2D	(D dst, const size_t pitch, const T value, const size_t w, const size_t h, VEDAstream stream);
template<typename D>			void memset	(D dst, const uint64_t x, const uint64_t y, const size_t size, VEDAstream stream);
template<typename D>			void memset2D	(D dst, const size_t pitch, const uint64_t x, const uint64_t y, const size_t w, const size_t h, VEDAstream stream);
};

//------------------------------------------------------------------------------
	}
}
