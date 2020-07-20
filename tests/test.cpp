#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#define CHECK(err) check(err, __FILE__, __LINE__)

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

	VEDAcontext ctx;
	CHECK(vedaCtxCreate(&ctx, 0, 0));
	printf("vedaCtxCreate(%p, %i, %i)\n", ctx, 0, 0);

	int devcnt;
	CHECK(vedaDeviceGetCount(&devcnt));
	printf("vedaDeviceGetCount(%i)\n", devcnt);

	char name[128];
	CHECK(vedaDeviceGetName(name, sizeof(name), 0));
	printf("veDeviceGetName(\"%s\", %i, %i)\n", name, sizeof(name), 0);

	for(int i = 0; i < 8; i++) {
		float temp;
		CHECK(vedaDeviceGetTemp(&temp, i, 0));
		printf("vedaDeviceGetTemp(%f, %i, %i)\n", temp, i, 0);
	}

	size_t memTotal = 0, memFree = 0;
	CHECK(vedaMemGetInfo(&memFree, &memTotal));
	printf("vedaMemGetInfo(%llu, %llu)\n", memFree, memTotal);

	float power;
	CHECK(vedaDeviceGetPower(&power, 0));
	printf("vedaDeviceGetPower(%f, %i)\n", power, 0);

	int value;
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_ABI_VERSION, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_ABI_VERSION, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO, 0)\n", value);
	CHECK(vedaDeviceGetAttribute(&value, VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION, 0));
	printf("vedaDeviceGetAttribute(%i, VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION, 0)\n", value);

	size_t cnt = 128;
	size_t size = cnt * sizeof(int);
	int* host = (int*)malloc(size);
	for(size_t i = 0; i < cnt; i++)
		host[i] = i;

	for(size_t i = 0; i < cnt; i++)
		if(host[i] != i)
			exit(1);

	VEDAdeviceptr ptr;
	CHECK(vedaMemAllocAsync(&ptr, size, 0));
	printf("vedaMemAllocAsync(%p, %llu, %i)\n", ptr, size, 0);

	CHECK(vedaMemcpyHtoDAsync(ptr, host, size, 0));
	printf("vedaMemcpyHtoDAsync(%p, %p, %llu, %i)\n",	ptr, host, size, 0);

	CHECK(vedaMemsetD32Async(ptr, 0xDEADBEEF, cnt, 0));
	printf("vedaMemsetD32Async(%p, %08X, %llu, %i)\n", ptr, 0xDEADBEEF, cnt, 0);

	CHECK(vedaMemcpyDtoHAsync(host, ptr, size, 0));
	printf("vedaMemcpyDtoHAsync(%p, %p, %llu, %i)\n",	host, ptr, size, 0);

	CHECK(vedaCtxSynchronize());
	printf("vedaCtxSynchronize()\n");

	for(size_t i = 0; i < cnt; i++)
		if(host[i] != 0xDEADBEEF)
			return 1;

	VEDAmodule mod;
	const char* modName = "libveda_test.vso";
	CHECK(vedaModuleLoad(&mod, modName));
	printf("vedaModuleLoad(%p, \"%s\")\n", mod, modName);

	VEDAfunction func;
	const char* funcName = "ve_test";
	CHECK(vedaModuleGetFunction(&func, mod, funcName));
	printf("vedaModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);
	
	CHECK(vedaCtxSynchronize());
	printf("vedaCtxSynchronize()\n");

	VEDAdeviceptr ptr2;
	CHECK(vedaMemAllocAsync(&ptr2, 0, 0));
	CHECK(vedaLaunchKernel(func, 0, ptr, ptr2, cnt));
	printf("vedaLaunchKernel(%p, %i, %p, %p, %llu)\n", func, 0, ptr, ptr2, cnt);

	CHECK(vedaMemcpyDtoHAsync(host, ptr2, size, 0));
	printf("vedaMemcpyDtoHAsync(%p, %p, %llu, %i)\n", host, ptr2, size, 0);

	CHECK(vedaCtxSynchronize());
	printf("vedaCtxSynchronize()\n");

	for(size_t i = 0; i < cnt; i++)
		if(host[i] != (cnt - 1 - i))
			return 1;

	CHECK(vedaModuleUnload(mod));
	printf("vedaModuleUnload(%p)\n", mod);
	CHECK(vedaMemFreeAsync(ptr2, 0));
	printf("vedaMemFree(%p, %i)\n", ptr, 0);
	CHECK(vedaCtxDestroy(ctx));
	printf("vedaCtxDestroy(%p)\n", ctx);

	free(host);

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}