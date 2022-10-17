#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#define CHECK(err) check(err, __FILE__, __LINE__)
#define SYNC_ALL true

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char* name = 0;
		vedaGetErrorName(err, &name);
		printf("Error: %i %s @ %s (%i)\n", err, name, file, line);
		assert(false);
		exit(1);
	}
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	int devcnt;
	CHECK(vedaDeviceGetCount(&devcnt));
	printf("vedaDeviceGetCount(%i)\n", devcnt);

	for(int dev = 0; dev < devcnt; dev++) {
		printf("\n# ------------------------------------- #\n");
		printf("# RUNNING TESTS ON %i                    #\n", dev);
		printf("# ------------------------------------- #\n\n");

		VEDAcontext ctx;
		CHECK(vedaCtxCreate(&ctx, 0, dev));
		printf("vedaCtxCreate(%p, %i, %i)\n", ctx, 0, dev);

		char name[128];
		CHECK(vedaDeviceGetName(name, sizeof(name), dev));
		printf("vedaDeviceGetName(\"%s\", %i, %i)\n", name, sizeof(name), dev);

		int cores;
		CHECK(vedaDeviceGetAttribute(&cores, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, dev));
		for(int i = 0; i < cores; i++) {
			float temp;
			CHECK(vedaDeviceGetTemp(&temp, i, dev));
			printf("vedaDeviceGetTemp(%f, %i, %i)\n", temp, i, dev);
		}

		size_t memTotal = 0, memFree = 0;
		CHECK(vedaMemGetInfo(&memFree, &memTotal));
		printf("vedaMemGetInfo(%llu, %llu)\n", memFree, memTotal);

		float power;
		CHECK(vedaDeviceGetPower(&power, dev));
		printf("vedaDeviceGetPower(%f, %i)\n", power, dev);

		int value;
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_ABI_VERSION, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_ABI_VERSION, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO, %i)\n", value, dev);
		CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION, dev));
		printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION, %i)\n", value, dev);

		size_t cnt = 128;
		size_t size = cnt * sizeof(int);
		int* host = (int*)malloc(size);
		for(size_t i = 0; i < cnt; i++)
			host[i] = i;

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != i) {
				printf("expected host[%i] to be %p but is %p\n", i, i, host[i]);
				return 1;
			}
		}

		VEDAptr<int> ptr;
		CHECK(vedaMemAllocAsync(&ptr, size, 0));
		printf("vedaMemAllocAsync(%p, %llu, %i)\n", (VEDAdeviceptr)ptr, size, 0);

		CHECK(vedaMemcpyHtoDAsync(ptr, host, size, 0));
		printf("vedaMemcpyHtoDAsync(%p, %p, %llu, %i)\n", (VEDAdeviceptr)ptr, host, size, 0);

		CHECK(vedaMemsetD32Async(ptr, 0xDEADBEEF, cnt, 0));
		printf("vedaMemsetD32Async(%p, %08X, %llu, %i)\n", (VEDAdeviceptr)ptr, 0xDEADBEEF, cnt, 0);

		CHECK(vedaMemcpyDtoHAsync(host, ptr, size, 0));
		printf("vedaMemcpyDtoHAsync(%p, %p, %llu, %i)\n", host, (VEDAdeviceptr)ptr, size, 0);

		CHECK(vedaCtxSynchronize());
		printf("vedaCtxSynchronize()\n");

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != 0xDEADBEEF) {
				printf("expected host[%i] to be %p but is %p\n", i, 0xDEADBEEF, host[i]);
				return 1;
			}
		}

		VEDAmodule mod;
		const char* modName = "libveda_test.vso";
		CHECK(vedaModuleLoad(&mod, modName));
		printf("vedaModuleLoad(%p, \"%s\")\n", mod, modName);

		VEDAfunction func;
		const char* funcName = "ve_test";
		CHECK(vedaModuleGetFunction(&func, mod, funcName));
		printf("vedaModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);

		VEDAdeviceptr ptr2;
		CHECK(vedaMemAllocAsync(&ptr2, 0, 0));
		printf("vedaMemAllocAsync(%p, %llu, %i)\n", ptr2, 0, 0);

		uint64_t res = 0xDEADBEEF0C0FFEE0llu;
		CHECK(vedaLaunchKernelEx(func, 0, &res, ptr.ptr(), ptr2, cnt));
		printf("vedaLaunchKernelEx(%p, %i, %p, %p, %llu, %016llX)\n", func, 0, (VEDAdeviceptr)ptr, ptr2, cnt, res);

		CHECK(vedaMemcpyDtoHAsync(host, ptr2, size, 0));
		printf("vedaMemcpyDtoHAsync(%p, %p, %llu, %i)\n", host, ptr2, size, 0);
		
		CHECK(vedaCtxSynchronize());
		printf("vedaCtxSynchronize()\n");
		printf("res == %016llX\n", res);

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != (cnt - 1 - i)) {
				printf("expected host[%i] to be %p but is %p\n", i, (cnt - 1 - i), host[i]);
				return 1;
			}
		}

		CHECK(vedaModuleUnload(mod));
		printf("vedaModuleUnload(%p)\n", mod);


		CHECK(vedaMemFreeAsync(ptr, 0));
		printf("vedaMemFree(%p, %i)\n", (VEDAdeviceptr)ptr, 0);

		CHECK(vedaMemFreeAsync(ptr2, 0));
		printf("vedaMemFree(%p, %i)\n", ptr2, 0);

		CHECK(vedaCtxDestroy(ctx));
		printf("vedaCtxDestroy(%p)\n", ctx);

		free(host);
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}