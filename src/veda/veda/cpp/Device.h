#pragma once

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
class Device final {
public:
	typedef std::vector<int>	Cores;

private:
	internal::Device* const	m_device;
	const VEDAcontext_mode	m_mode;

				Device			(VEDAcontext ctx);
	Context			ctx			(const bool checkIsActive = true) const;
	template<typename B> B	alloc_			(const size_t size, const StreamId stream) const;

public:
				Device			(int ordinal, const VEDAcontext_mode mode = VEDA_CONTEXT_MODE_OMP);
	Module			load			(const std::string_view name) const;
	VEDAcontext_mode	mode			(void) const;
	bool			isActive		(void) const;
	bool			isNUMA			(void) const;
	const Cores&		coreIds			(void) const;
	float			current			(void) const;
	float			currentEdge		(void) const;
	float			distance		(const Device& other) const;
	float			power			(void) const;
	float			temp			(const int coreIdx) const;
	float			voltage			(void) const;
	float			voltageEdge		(void) const;
	int			abi			(void) const;
	int			aveoId			(void) const;
	int			cacheL1d		(void) const;
	int			cacheL1i		(void) const;
	int			cacheL2			(void) const;
	int			cacheLLC		(void) const;
	int			clockBase		(void) const;
	int			clockMemory		(void) const;
	int			clockRate		(void) const;
	int			cores			(void) const;
	int			firmware		(void) const;
	int			model			(void) const;
	int			numaCnt			(void) const;
	int			numaId			(void) const;
	int			physicalId		(void) const;
	int			singleToDoublePerfRatio	(void) const;
	int			streamCnt		(void) const;
	int			vedaId			(void) const;
	size_t			numaMemBlockSize	(void) const;
	size_t			totalMem		(void) const;
	size_t			usedMem			(void) const;
	std::string		name			(void) const;
	void			destroy			(void) const;
	void			pushCurrent		(void) const;
	void			setCurrent		(void) const;
	void			sync			(const StreamId stream = -1) const;

	static	Device		getCurrent		(void);
	static	Device		popCurrent		(void);
	static	int		count			(void);

	template<typename T, typename B=VEDA>
	inline Ptr<B, T> alloc(const size_t cnt, const StreamId stream = 0) const {
		return {alloc_<B>(cnt * sizeof(T), stream), cnt, stream};
	}

	friend class HostFunctionBase;
};

//------------------------------------------------------------------------------
	}
}