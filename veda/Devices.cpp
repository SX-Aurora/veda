#include "veda.hpp"

namespace veda {
//------------------------------------------------------------------------------
std::vector<Device> Devices::s_devices;

//------------------------------------------------------------------------------
int	Devices::count		(void) {	return (int)s_devices.size();	}
void	Devices::shutdown	(void) {	s_devices.clear();		}

//------------------------------------------------------------------------------
void Devices::memReport(void) {
	for(auto& d : s_devices)
		d.memReport();
}

//------------------------------------------------------------------------------
void Devices::report(void) {
	for(auto& d : s_devices)
		d.report();
}

//------------------------------------------------------------------------------
Device& Devices::get(const VEDAdeviceptr ptr) {
	Ptr vptr(ptr);
	return get(vptr.device());
}

//------------------------------------------------------------------------------
Device& Devices::get(const VEDAdevice device) {
	if(device < 0 || device >= s_devices.size())
		throw VEDA_ERROR_INVALID_DEVICE;
	return s_devices[device];
}

//------------------------------------------------------------------------------
void Devices::init(void) {
	std::set<int> devices;
	initCount	(devices);
	initMapping	(devices);
}

//------------------------------------------------------------------------------
void Devices::initCount(std::set<int>& devices) {
	assert(devices.empty());

	struct dirent* dp;
	DIR* fd;

	if((fd = opendir("/dev/")) == NULL)
		return;

	while((dp = readdir(fd)) != NULL) {
		if(!strncmp(dp->d_name, "veslot", 6)) {
			auto it = devices.emplace(std::atoi(dp->d_name + 6));
			assert(it.second);
		}
	}

	closedir(fd);
}

//------------------------------------------------------------------------------
void Devices::initMapping(const std::set<int>& devices) {
	if(devices.empty())
		return;

	std::set<int> visible;
	auto env = std::getenv("VEDA_VISIBLE_DEVICES");
	if(!env)
		env = std::getenv("VE_NODE_NUMBER");

	if(env) {
		std::istringstream ss(env);
		std::string line;
		while(ss.good()) {
			std::getline(ss, line, ',');
			if(line.size())
				visible.emplace(std::atoi(line.c_str()));
			line.clear();
		}
	}

	// Parse real device ids
	char device[] = "/dev/veslotX";
	for(int deviceIdx : devices) {
		if(!visible.empty() && visible.find(deviceIdx) == visible.end())
			continue;

		assert(deviceIdx < 10); // otherwise this will fail
		device[strlen(device) - 1] = '0' + (char)deviceIdx;

		struct stat sb = {0};
		if(stat(device, &sb) == -1)
			throw VEDA_ERROR_INITIALIZING_DEVICE;

		udev* udev = udev_new();
		auto ve_udev = udev_device_new_from_devnum(udev, 'c', sb.st_rdev);
		if(!ve_udev)
			throw VEDA_ERROR_INITIALIZING_DEVICE;

		auto sysfs_path = udev_device_get_syspath(ve_udev);
		if(!sysfs_path)
			throw VEDA_ERROR_INITIALIZING_DEVICE;

		char real_device_idx = sysfs_path[strlen(sysfs_path)-1];
		udev_device_unref(ve_udev);
		udev_unref(udev);

		auto aveoId	= deviceIdx;
		auto sensorId	= (int)(real_device_idx - '0');
		int numaCnt	= 1;

		if(readSensor(sensorId, "partitioning_mode", false))
			numaCnt = 2;

		for(int numaId = 0; numaId < numaCnt; numaId++) {
			auto vedaId = (VEDAdevice)s_devices.size();
			s_devices.emplace_back(vedaId, aveoId, sensorId, numaId);
		}
	}
}

//------------------------------------------------------------------------------
uint64_t Devices::readSensor(const int sensorId, const char* file, const bool isHex) {
	if(file == 0)
		throw VEDA_ERROR_NO_SENSOR_FILE;

	char buffer[128];
	snprintf(buffer, sizeof(buffer), "/sys/class/ve/ve%i/%s", sensorId, file);

	uint64_t value = 0;
	std::ifstream f(buffer, std::ios::binary);
	if(!f.good())
		throw VEDA_ERROR_CANT_READ_SENSOR_FILE;
	if(isHex)	f >> std::hex >> value;
	else		f >> value;
	f.close();

	return value;
}


//------------------------------------------------------------------------------
}