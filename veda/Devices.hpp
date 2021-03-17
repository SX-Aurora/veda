#pragma once

namespace veda {
	class Devices {
		static std::vector<Device> s_devices;

		static	void	initCount	(std::set<int>& devices);
		static	void	initMapping	(const std::set<int>& devices);

	public:
		static	int	count		(void);
		static	void	shutdown	(void);
		static	Device&	get		(const VEDAdeviceptr ptr);
		static	Device&	get		(const VEDAdevice device);
		static	void	init		(void);
		static	void	memReport	(void);
	};
}