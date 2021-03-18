#pragma once

namespace veda {
	class Devices {
		static std::vector<Device> s_devices;

		static	void		initCount	(std::set<int>& devices);
		static	void		initMapping	(const std::set<int>& devices);

	public:
		static	Device&		get		(const VEDAdevice device);
		static	Device&		get		(const VEDAdeviceptr ptr);
		static	int		count		(void);
		static	uint64_t	readSensor	(const int sensorId, const char* file, const bool isHex);
		static	void		init		(void);
		static	void		memReport	(void);
		static	void		report		(void);
		static	void		shutdown	(void);
	};
}