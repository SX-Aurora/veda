#include "veda/veda.hpp"

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaDeviceGet(VEDAdevice* device, int ordinal) {
	GUARDED(
		veda::Devices::get(ordinal);
		*device = ordinal;
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetTemp(float* tempC, const int coreIdx, VEDAdevice dev) {
	GUARDED(*tempC = veda::Devices::get(dev).coreTemp(coreIdx);)
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetPower(float* watts, VEDAdevice dev) {
	GUARDED(
		auto& device = veda::Devices::get(dev);
		*watts = device.powerVoltage() * device.powerCurrent();
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetAttribute(int* pi, VEDAdevice_attribute attrib, VEDAdevice dev) {
	GUARDED(
		if(attrib == VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO) {
			*pi = 2;
		} else {
			auto& device = veda::Devices::get(dev);
			*pi = [&](void) -> int {
				switch(attrib) {
					case VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE:			return device.clockRate();
					case VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE:			return device.clockBase();
					case VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT:	return device.cores();
					case VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE:		return device.clockMemory();
					case VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE:		return device.cacheL1d();
					case VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE:		return device.cacheL1i();
					case VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE:		return device.cacheL2();
					case VEDA_DEVICE_ATTRIBUTE_ABI_VERSION:			return device.versionAbi();
					case VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION:		return device.versionFirmware();
				}

				throw VEDA_ERROR_UNKNOWN_ATTRIBUTE;
				return 0;
			}();
		}
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetCount(int* count) {
	GUARDED(*count = veda::Devices::count();)
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
	GUARDED(*bytes = veda::Devices::get(dev).memorySize();)
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxGetState(VEDAdevice dev, uint32_t* flags, int* active) {
	GUARDED(
		VEDAcontext ctx;
		CVEDA(vedaCtxGet(&ctx, dev));
		*flags	= 0;
		*active = ctx != 0;
	)
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
