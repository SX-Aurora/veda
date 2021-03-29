#pragma once

namespace veda {
	class Device final {
		typedef std::vector<int>	Cores;

			Cores			m_cores;
		const	VEDAdevice		m_vedaId;
		const	int			m_aveoId;
		const	int			m_sensorId;
		const	int			m_numaId;
		const	bool			m_isNUMA;
		const	int			m_memorySize;
		const	int			m_clockRate;
		const	int			m_clockBase;
		const	int			m_clockMemory;
		const	int			m_cacheL1d;
		const	int			m_cacheL1i;
		const	int			m_cacheL2;
		const	int			m_versionAbi;
		const	int			m_versionFirmware;
			Context*		m_ctx;

		uint64_t	readSensor	(const char* file, const bool isHex) const;

		template<typename T>
		inline T readSensor(const char* file, const bool isHex = false) const {
			return static_cast<T>(readSensor(file, isHex));
		}

	public:
				Device		(const VEDAdevice vedaId, const int aveoId, const int sensorId, const int numaId);
				~Device		(void);
		Context*	createCtx	(const VEDAcontext_mode mode);
		Context*	ctx		(void) const;
		VEDAdevice	vedaId		(void) const;
		float		coreTemp	(const int core) const;
		float		powerCurrent	(void) const;
		float		powerVoltage	(void) const;
		bool		isNUMA		(void) const;
		int		aveoId		(void) const;
		int		cacheL1d	(void) const;
		int		cacheL1i	(void) const;
		int		cacheL2		(void) const;
		int		clockBase	(void) const;
		int		clockMemory	(void) const;
		int		clockRate	(void) const;
		int		cores		(void) const;
		int		memorySize	(void) const;
		int		numaId		(void) const;
		int		sensorId	(void) const;
		int		versionAbi	(void) const;
		int		versionFirmware	(void) const;
		void		destroyCtx	(void);
		void		memReport	(void) const;
		void		report		(void) const;
	};
}