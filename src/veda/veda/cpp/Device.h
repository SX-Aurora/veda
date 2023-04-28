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
						Context	ctx			(const bool checkIsActive = true) const;
	template<typename B> 			B	alloc_			(const size_t size, const StreamId stream) const;
	template<typename B> 			void	free_			(const B ptr, const StreamId stream) const;
	template<typename D, typename S>	void	memcpy_			(D dst, S src, const size_t size, const StreamId stream) const;
	template<typename D, typename B>	void	memset_			(B ptr, const D value, const size_t cnt, const StreamId stream) const;

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
	VEDAresult		query			(const StreamId stream = 0) const;

	static	Device		getCurrent		(void);
	static	Device		popCurrent		(void);
	static	int		count			(void);

	template<typename D, typename S>
	inline void memcpy(D dst, S src, const size_t size, const StreamId stream = 0) const {
		using Dcast = typename std::conditional<std::is_same<D, VEDA>::value || std::is_same<D, HMEM>::value, D, void*>::type;
		using Scast = typename std::conditional<std::is_same<S, VEDA>::value || std::is_same<S, HMEM>::value, S, void*>::type;
		static_assert(std::is_pointer<D>::value);
		static_assert(std::is_pointer<S>::value);
		memcpy_<Dcast, Scast>((Dcast)dst, (Scast)src, size, stream);
	}

	template<typename T, typename B>
	inline typename std::enable_if<sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16>::type memset(B ptr, const T value, const size_t cnt, const StreamId stream = 0) const {
		if	constexpr (sizeof(T) == 1)	memset_<>(ptr, *(const uint8_t *)&value, cnt, stream);
		else if	constexpr (sizeof(T) == 2)	memset_<>(ptr, *(const uint16_t*)&value, cnt, stream);
		else if	constexpr (sizeof(T) == 4)	memset_<>(ptr, *(const uint32_t*)&value, cnt, stream);
		else if	constexpr (sizeof(T) == 8)	memset_<>(ptr, *(const uint64_t*)&value, cnt, stream);
		// TODO: else if	constexpr (sizeof(T) == 16)	memset_<>(ptr, (const int64_t*)&value, cnt, stream);
	}

	template<typename T=int8_t, typename B=VEDA>
	inline B allocRaw(const size_t cnt, const StreamId stream = 0) const {
		return alloc_<B>(cnt * sizeof(T), stream);
	}

	template<typename T=int8_t, typename B=VEDA>
	inline Ptr<B, T> alloc(const size_t cnt, const StreamId stream = 0) const {
		return {allocRaw<T, B>(cnt, stream), cnt, stream};
	}

	template<typename B>
	inline typename std::enable_if<std::is_same<B, VEDA>::value || std::is_same<B, HMEM>::value>::type free(const B ptr, const StreamId stream = 0) const {
		free_(ptr, stream);
	}

	friend class HostFunctionBase;
};

//------------------------------------------------------------------------------
	}
}