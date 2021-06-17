#include "veda/veda.hpp"

extern "C" {
//------------------------------------------------------------------------------
VEDAresult	vedaCtxGet			(VEDAcontext* ctx, const VEDAdevice device)		{	GUARDED(*ctx = veda::Devices::get(device).ctx();)		}
VEDAresult	vedaDeviceGetTemp		(float* tempC, const int coreIdx, VEDAdevice dev)	{	GUARDED(*tempC = veda::Devices::get(dev).coreTemp(coreIdx);)	}
VEDAresult	vedaDeviceGetCount		(int* count)						{	GUARDED(*count = veda::Devices::count();)			}
VEDAresult	vedaDeviceTotalMem		(size_t* bytes, VEDAdevice dev)				{	GUARDED(*bytes = veda::Devices::get(dev).memorySize();)		}
VEDAresult	vedaDevicePrimaryCtxRelease	(VEDAdevice dev)					{	return VEDA_SUCCESS;						}
VEDAresult	vedaDevicePrimaryCtxSetFlags	(VEDAdevice dev, uint32_t flags)			{	return VEDA_SUCCESS;						}
VEDAresult	vedaDeviceGetPhysicalId		(int* id, VEDAdevice dev)				{	GUARDED(*id = veda::Devices::get(dev).sensorId();)		}
VEDAresult	vedaDeviceGetAVEOId		(int* id, VEDAdevice dev)				{	GUARDED(*id = veda::Devices::get(dev).aveoId();)		}
VEDAresult	vedaDeviceGetNUMAId		(int* id, VEDAdevice dev)				{	GUARDED(*id = veda::Devices::get(dev).numaId();)		}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceDistance(float* distance, VEDAdevice devA, VEDAdevice devB) {
	GUARDED(
		*distance = 0.0f;

		auto& A = veda::Devices::get(devA);
		auto& B = veda::Devices::get(devB);
		
		if(A.vedaId() == B.vedaId()) {
			*distance = 0.0f;
		} else if(A.aveoId() == B.aveoId()) {
			ASSERT(A.numaId() != B.numaId());
			*distance = 0.5f;
		} else {
			*distance = 1.0f;
		}
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGet(VEDAdevice* device, int ordinal) {
	GUARDED(
		veda::Devices::get(ordinal);
		*device = ordinal;
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetPower(float* watts, VEDAdevice dev) {
	GUARDED(
		auto& device = veda::Devices::get(dev);
		*watts = device.powerVoltage() * device.powerCurrent();
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDeviceGetCurrent(float* current, VEDAdevice dev) { GUARDED(*current = veda::Devices::get(dev).powerCurrent();)	}
VEDAresult vedaDeviceGetVoltage(float* voltage, VEDAdevice dev) { GUARDED(*voltage = veda::Devices::get(dev).powerVoltage();)	}

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
					case VEDA_DEVICE_ATTRIBUTE_LLC_CACHE_SIZE:		return device.cacheLLC();
					case VEDA_DEVICE_ATTRIBUTE_MODEL:			return device.model();
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
VEDAresult vedaDeviceGetName(char* name, int len, VEDAdevice dev) {
	GUARDED(
		auto& device = veda::Devices::get(dev);
		auto version		= device.model();
		auto cores		= device.cores();
		auto memory		= device.memorySize()/1024/1024/1024;
		auto clock_memory	= device.clockMemory();
		auto clock_rate		= device.clockRate();

		const char* model = [&] {
			if(version == 1) {
				if(memory == 48) {
					if(clock_rate >= 1500)	return clock_memory < 1700 ? "10A" : "10AE";
					else			return clock_memory < 1700 ? "10B" : "10BE";
				} else if(memory == 24) {
					return clock_memory < 1700 ? "10C" : "10CE";
				}
			} else if(version == 2) {
				if(cores == 5 || cores == 10)	return "20A";
				if(cores == 4 || cores == 8)	return "20B";
			}
			return "?";
		}();
		snprintf(name, len, "NEC SX-Aurora Tsubasa VE%s", model);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxGetState(VEDAdevice dev, uint32_t* flags, int* active) {
	GUARDED(
		*flags	= 0;
		*active = veda::Devices::get(dev).ctx() != 0;
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxReset(VEDAdevice dev) {
	GUARDED(
		auto& device = veda::Devices::get(dev);
		if(auto ctx = device.ctx()) {
			auto mode = ctx->mode();
			device.destroyCtx();
			device.createCtx(mode);
		}
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDevicePrimaryCtxRetain(VEDAcontext* pctx, VEDAdevice dev) {
	GUARDED(
		auto& device = veda::Devices::get(dev);
		if(!device.ctx())
			device.createCtx(VEDA_CONTEXT_MODE_OMP);
		*pctx = device.ctx();
	)
}

//------------------------------------------------------------------------------
} // extern "C"
