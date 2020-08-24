#include "veda_internal.h"

//------------------------------------------------------------------------------
static std::vector<std::tuple<int, int>> veda_devices;

//-----------------------------------------------------------------------------
static uint64_t read(const char* file) {
	uint64_t value = 0;
	std::ifstream f(file, std::ios::binary);
	if(f.good());
		f >> value;
	f.close();
	return value;
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceExit(void) {
	veda_devices.clear();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetSensorIdx(int* idx, const VEDAdevice device) {
	CVEDA(vedaIsDevice(device));
	*idx = std::get<0>(veda_devices[device]);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetPhysicalIdx(int* idx, const VEDAdevice device) {
	CVEDA(vedaIsDevice(device));
	*idx = std::get<1>(veda_devices[device]);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetInfo(uint64_t* value, const char* file, const VEDAdevice device) {
	CVEDA(vedaIsDevice(device));
	
	if(file == 0)
		return VEDA_ERROR_NO_SENSOR_FILE;

	int idx = -1;
	CVEDA(vedaDeviceGetSensorIdx(&idx, device)); assert(idx >= 0);
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "/sys/class/ve/ve%i/%s", idx, file);
	*value = read(buffer);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaIsDevice(const VEDAdevice device) {
	return device >= 0 && device < veda_devices.size() ? VEDA_SUCCESS : VEDA_ERROR_INVALID_DEVICE;
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceInitCount(std::set<int>& devices) {
	assert(devices.empty());

	struct dirent* dp;
	DIR* fd;

	if((fd = opendir("/dev/")) == NULL)
		return VEDA_SUCCESS;

	while((dp = readdir(fd)) != NULL) {
		if(!strncmp(dp->d_name, "veslot", 6)) {
			auto it = devices.emplace(std::atoi(dp->d_name + 6));
			assert(it.second);
		}
	}

	closedir(fd);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceInitMapping(const std::set<int>& devices) {
	if(devices.empty())
		return VEDA_SUCCESS;

	// Parse VEDA_VISIBLE_DEVICES
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
			return VEDA_ERROR_INITIALIZING_DEVICE;

		udev* udev = udev_new();
		auto ve_udev = udev_device_new_from_devnum(udev, 'c', sb.st_rdev);
		if(!ve_udev)
			return VEDA_ERROR_INITIALIZING_DEVICE;

		auto sysfs_path = udev_device_get_syspath(ve_udev);
		if(!sysfs_path)
			return VEDA_ERROR_INITIALIZING_DEVICE;

		char real_device_idx = sysfs_path[strlen(sysfs_path)-1];
		udev_device_unref(ve_udev);
		udev_unref(udev);

		veda_devices.emplace_back((int)(real_device_idx - '0'), deviceIdx);
	}

	return VEDA_SUCCESS;
}
//------------------------------------------------------------------------------

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaDeviceGet(VEDAdevice* device, int ordinal) {
	GUARDED(
		*device = ordinal;
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetTemp(float* tempC, const int coreIdx, VEDAdevice dev) {
	GUARDED(
		if(coreIdx < 0 || coreIdx >= 8)	return VEDA_ERROR_INVALID_COREIDX;
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "sensor_%i", coreIdx + 14);

		uint64_t value;
		CVEDA(vedaDeviceGetInfo(&value, buffer, dev));

		*tempC = value/1000000.0f;
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetPower(float* watts, VEDAdevice dev) {
	GUARDED(
		uint64_t iv, ic;
		CVEDA(vedaDeviceGetInfo(&iv, "sensor_8", dev));	// is in uV
		CVEDA(vedaDeviceGetInfo(&ic, "sensor_12", dev)); // is in mA
		*watts = (iv/1000000.0f) * (ic/1000.0f);
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetAttribute(int* pi, VEDAdevice_attribute attrib, VEDAdevice dev) {
	GUARDED(
		CVEDA(vedaIsDevice(dev));

		if(attrib == VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO) {
			*pi = 2;
		} else {
			const char* sensor = [&](void) -> const char* {
				switch(attrib) {
					case VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE:				return "clock_chip";
					case VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE:				return "clock_base";
					case VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT:	return "num_of_core";
					case VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE:		return "clock_memory";
					case VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE:			return "cache_l1d";
					case VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE:			return "cache_l1i";
					case VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE:			return "cache_l2";
					case VEDA_DEVICE_ATTRIBUTE_ABI_VERSION:				return "abi_version";
					case VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION:	 	return "fw_version";
				}

				return 0;
			}();
				
			uint64_t value;
			CVEDA(vedaDeviceGetInfo(&value, sensor, dev));

			*pi = (int)value;
		}
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetCount(int* count) {
	*count = (int)veda_devices.size();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetName(char* name, int len, VEDAdevice dev) {
	GUARDED(
		int abi;
		CVEDA(vedaDeviceGetAttribute(&abi, VEDA_DEVICE_ATTRIBUTE_ABI_VERSION, dev));
		snprintf(name, len, "NEC SX-Aurora Tsubasa VE%i", abi);
		// TODO: get VE10Ae, VE10A etc. special names?
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceTotalMem(size_t* bytes, VEDAdevice dev) {
	GUARDED(
		uint64_t value = 0;
		CVEDA(vedaDeviceGetInfo(&value, "memory_size", dev));
		*bytes = value * 1024 * 1024 * 1024;
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxGetState(VEDAdevice dev, uint32_t* flags, int* active) {
	GUARDED(
		VEDAcontext ctx;
		CVEDA(vedaCtxGet(&ctx, dev));
		*flags	= 0;
		*active = ctx != 0;
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxRelease(VEDAdevice dev) {
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxReset(VEDAdevice dev) {
	GUARDED(
		VEDAcontext ctx;
		CVEDA(vedaCtxGet(&ctx, dev));
		if(ctx)
			CVEDA(vedaCtxDestroy(ctx));
		CVEDA(vedaCtxCreate(&ctx, 0, dev));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxRetain(VEDAcontext* pctx, VEDAdevice dev) {
	CVEDA(vedaCtxGet(pctx, dev));
	if(*pctx == 0)
		CVEDA(vedaCtxCreate(pctx, 0, dev));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxSetFlags(VEDAdevice dev, uint32_t flags) {
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
} // extern "C"