#include <veda/internal/api.h>
#include <veda/internal/hmem_macros.h>
#include <regex>

#define SYS_CLASS_VE_BUFFER_SIZE 128

namespace veda {
	namespace internal {
		namespace devices {
//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------
static std::deque<Device>	s_devices;
static std::vector<Device*>	s_mapping(VEO_MAX_HMEM_PROCS);

//------------------------------------------------------------------------------
static void initCount(std::set<int>& devices) {
	assert(devices.empty());

	struct dirent* dp = 0;
	DIR* fd = 0;

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
static int countNUMA(const int sensorId) {
	char buffer[SYS_CLASS_VE_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "/sys/class/ve/ve%i", sensorId);

	struct dirent* dp = 0;
	DIR* fd = 0;

	if((fd = opendir(buffer)) == NULL)
		VEDA_THROW(VEDA_ERROR_INVALID_DEVICE);

	const std::regex regex("numa[0-9]+_cores");
	std::cmatch match;

	int cnt = 0;
	while((dp = readdir(fd)) != NULL) {
		if(std::regex_match(dp->d_name, match, regex))
			cnt++;
	}

	closedir(fd);
	return cnt;
}

//------------------------------------------------------------------------------
static void initMapping(const std::set<int>& devices) {
	if(devices.empty())
		return;

	// Parse VEDA_VISIBLE_DEVICES and VE_NODE_NUMBER as fallback
	std::set<std::tuple<int, int>> visible;
	auto ve_node_list		= std::getenv("_VENODELIST");
	auto veda_visible_devices	= std::getenv("VEDA_VISIBLE_DEVICES");
	
	if(!ve_node_list && !veda_visible_devices)
		veda_visible_devices = std::getenv("VE_NODE_NUMBER");

	if(veda_visible_devices) {
		std::istringstream ss(veda_visible_devices);
		std::string line;
		while(ss.good()) {
			std::getline(ss, line, ',');
			if(line.size()) {
				auto id		= (int)(std::atof(line.c_str()) * 10.0f);
				auto aveoId	= id / 10;
				auto numaId	= id % 10;
				if(aveoId >= 0)
					visible.emplace(aveoId, numaId);
			}
			line.clear();
		}
	}

	// Apply _VENODELIST
	std::map<int, int> mapping;
	if(ve_node_list) {
		std::istringstream ss(ve_node_list);
		std::string line;
		while(ss.good()) {
			std::getline(ss, line, ' ');
			if(line.size()) {
				auto id = std::atoi(line.c_str());
				mapping.emplace(id, (int)mapping.size());
			}
			line.clear();
		}

		// if _VENODELIST is set, but with no values, no VE's are active
		if(mapping.empty())
			return;
	}

	// Parse real device ids
	char device[] = "/dev/veslotX";
	for(int deviceIdx : devices) {
		assert(deviceIdx < 10); // otherwise this will fail

		// Check _VENODELIST filtering and determine AVEO ID
		auto it = mapping.find(deviceIdx);
		if(mapping.size() && it == mapping.end())
			continue;

		auto aveoId = it != mapping.end() ? it->second : deviceIdx;

		// Determine Sensor ID
		device[strlen(device) - 1] = '0' + (char)deviceIdx;

		struct stat sb = {0};
		if(stat(device, &sb) == -1)
			VEDA_THROW(VEDA_ERROR_INITIALIZING_DEVICE);

		udev* udev = udev_new();
		auto ve_udev = udev_device_new_from_devnum(udev, 'c', sb.st_rdev);
		if(!ve_udev)
			VEDA_THROW(VEDA_ERROR_INITIALIZING_DEVICE);

		auto sysfs_path = udev_device_get_syspath(ve_udev);
		if(!sysfs_path)
			VEDA_THROW(VEDA_ERROR_INITIALIZING_DEVICE);

		char real_device_idx = sysfs_path[strlen(sysfs_path)-1];
		udev_device_unref(ve_udev);
		udev_unref(udev);

		const auto sensorId = (int)(real_device_idx - '0');

		// Determine NUMA count
		const int numaCnt = readSensor(sensorId, "partitioning_mode", false) ? countNUMA(sensorId) : 1;

		for(int numaId = 0; numaId < numaCnt; numaId++) {
			if(visible.size() && visible.find({aveoId, numaId}) == visible.end())
				continue;

			auto vedaId = (VEDAdevice)s_devices.size();
			s_devices.emplace_back(vedaId, aveoId, sensorId, numaId, numaCnt);
		}
	}
}

//------------------------------------------------------------------------------
// Public
//------------------------------------------------------------------------------
int count(void) {
	return (int)s_devices.size();
}

//------------------------------------------------------------------------------
void shutdown(void) {
	/**
	 * Don't destroy CTX during shutdown, as it will try to sync pointers
	 * which can cause `ERROR: wait_req_ack() timeout waiting for ACK req=29`
	 */
	s_devices.clear();
	s_mapping.clear();
}

//------------------------------------------------------------------------------
void memReport(void) {
	for(auto& d : s_devices)
		d.ctx().memReport();
}

//------------------------------------------------------------------------------
void report(void) {
	for(auto& d : s_devices)
		d.report();
}

//------------------------------------------------------------------------------
Device& get(const VEDAdeviceptr vptr) {
	if(!vptr)
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	return get(VEDA_GET_DEVICE(vptr));
}

//------------------------------------------------------------------------------
Device& get(const VEDAdevice device) {
	if(device < 0 || device >= count())
		VEDA_THROW(VEDA_ERROR_INVALID_DEVICE);
	return s_devices[device];
}

//------------------------------------------------------------------------------
void init(void) {
	std::set<int> devices;
	initCount	(devices);
	initMapping	(devices);
}

//------------------------------------------------------------------------------
uint64_t readSensor(const int sensorId, const char* file, const bool isHex) {
	if(file == 0)
		VEDA_THROW(VEDA_ERROR_NO_SENSOR_FILE);

	char buffer[SYS_CLASS_VE_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "/sys/class/ve/ve%i/%s", sensorId, file);

	uint64_t value = 0;
	std::ifstream f(buffer, std::ios::binary);
	if(!f.good())
		VEDA_THROW(VEDA_ERROR_CANT_READ_SENSOR_FILE);
	if(isHex)	f >> std::hex >> value;
	else		f >> value;
	f.close();

	return value;
}

//------------------------------------------------------------------------------
int architecture(void) {
	static int s_architecture = 0;

	if(s_architecture == 0) {
		if(auto env = std::getenv("VEDA_ARCH")) {
			if(strlen(env) == 1) {
				switch(env[0]) {
					case '1':	s_architecture = 1; break;
					case '3':	s_architecture = 3; break;
				}
			}
		} else {
			std::string buf("/sys/class/ve/ve0/ve_arch_class");
			const auto pos = buf.find('0');
			for(char i = '0'; i <= '9'; i++) {
				buf[pos] = i;
				std::ifstream f(buf, std::ios::binary);
				if(f.fail())
					break;

				std::string str;
				f >> str;
				f.close();

				s_architecture = 3;
				if(str.compare("ve3") != 0) {
					s_architecture = 1;
					break;
				}
			}
		}
	}

	if(s_architecture == 0)
		VEDA_THROW(VEDA_ERROR_UNKNOWN_VEDA_ARCHITECTURE);
	return s_architecture;
}

//------------------------------------------------------------------------------
static Device* tryGet(const int aveoProcId) {
	if(aveoProcId < 0 || aveoProcId >= VEO_MAX_HMEM_PROCS)
		return 0;
	return s_mapping[aveoProcId];
}

//------------------------------------------------------------------------------
Device* tryGet(veo_proc_handle* handle) {
	return tryGet(veo_proc_identifier(handle));
}

//------------------------------------------------------------------------------
Device* tryGet(const VEDAhmemptr ptr) {
	if(!ptr || !IS_VE(ptr))
		return 0;
	return tryGet(GET_PROC_IDENT(ptr));
}

//------------------------------------------------------------------------------
Device& get(const VEDAhmemptr ptr) {
	if(auto ref = tryGet(ptr))
		return *ref;
	VEDA_THROW(VEDA_ERROR_INVALID_DEVICE);
}

//------------------------------------------------------------------------------
void map(const int aveoProcId, Device& dev) {
	ASSERT(aveoProcId >= 0 && aveoProcId < VEO_MAX_HMEM_PROCS);
	auto& ref = s_mapping[aveoProcId];
	assert(ref == 0);
	ref = &dev;
}

//------------------------------------------------------------------------------
void unmap(const int aveoProcId) {
	auto& ref = s_mapping[aveoProcId];
	assert(ref);
	ref = 0;
}

//------------------------------------------------------------------------------
		}
	}
}