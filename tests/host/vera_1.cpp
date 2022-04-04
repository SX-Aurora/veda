#include <vera.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#define CHECK(err) check(err, __FILE__, __LINE__)

void check(veraError_t err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		printf("Error: %i %s @ %s (%i)\n", err, veraGetErrorName(err), file, line);
		assert(false);
		exit(1);
	}
}

int main(int argc, char** argv) {
	int devcnt;
	CHECK(veraGetDeviceCount(&devcnt));
	printf("veraGetDeviceCount(%i)\n", devcnt);

	for(int dev = 0; dev < devcnt; dev++) {
		printf("\n# ------------------------------------- #\n");
		printf("# RUNNING TESTS ON %i                    #\n", dev);
		printf("# ------------------------------------- #\n\n");

		CHECK(veraSetDevice(dev));
		printf("veraSetDevice(%i)\n", dev);

		veraDeviceProp prop;
		CHECK(veraGetDeviceProperties(&prop, dev));
		printf("veraGetDeviceProperties(\"%s\", %i)\n", prop.name, 0);

		for(int i = 0; i < prop.multiProcessorCount; i++) {
			float temp;
			CHECK(veraDeviceGetTemp(&temp, i, dev));
			printf("veraDeviceGetTemp(%f, %i, %i)\n", temp, i, dev);
		}

		size_t memTotal = 0, memFree = 0;
		CHECK(veraMemGetInfo(&memFree, &memTotal));
		printf("veraMemGetInfo(%llu, %llu)\n", memFree, memTotal);

		float power;
		CHECK(veraDeviceGetPower(&power, 0));
		printf("veraDeviceGetPower(%f, %i)\n", power, 0);

		int value;
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrClockRate, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrClockRate, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrClockBase, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrClockBase, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrMultiProcessorCount, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrMultiProcessorCount, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrMemoryClockRate, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrMemoryClockRate, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrL1DCacheSize, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrL1DCacheSize, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrL1ICacheSize, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrL1ICacheSize, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrL2CacheSize, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrL2CacheSize, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrABIVersion, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrABIVersion, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrSingleToDoublePrecisionPerfRatio, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrSingleToDoublePrecisionPerfRatio, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrFirmwareVersion, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrFirmwareVersion, %i)\n", value, dev);

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

		void* ptr;
		CHECK(veraMallocAsync(&ptr, size, 0));
		printf("veraMallocAsync(%p, %llu, %i)\n", ptr, size, 0);

		CHECK(veraMemcpyAsync(ptr, host, size, veraMemcpyHostToDevice, 0));
		printf("veraMemcpyHtoDAsync(%p, %p, %llu, %i)\n", ptr, host, size, 0);

		CHECK(veraMemsetAsync(ptr, 0xDEADBEEF, cnt, 0));
		printf("veraMemsetAsync(%p, %08X, %llu, %i)\n", ptr, 0xDEADBEEF, cnt, 0);

		CHECK(veraMemcpyAsync(host, ptr, size, veraMemcpyDeviceToHost, 0));
		printf("veraMemcpyAsync(%p, %p, %llu, %i)\n", host, ptr, size, 0);

		CHECK(veraDeviceSynchronize());
		printf("veraDeviceSynchronize()\n");

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != 0xDEADBEEF) {
				printf("expected host[%i] to be %p but is %p\n", i, 0xDEADBEEF, host[i]);
				return 1;
			}
		}

		veraModule_t mod;
		const char* modName = "libveda_test.vso";
		CHECK(veraModuleLoad(&mod, modName));
		printf("veraModuleLoad(%p, \"%s\")\n", mod, modName);

		veraFunction_t func;
		const char* funcName = "ve_test";
		CHECK(veraModuleGetFunction(&func, mod, funcName));
		printf("veraModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);
		
		CHECK(veraDeviceSynchronize());
		printf("veraDeviceSynchronize()\n");

		void* ptr2;
		CHECK(veraMallocAsync(&ptr2, 0, 0));
		CHECK(veraLaunchKernel(func, 0, VEDAptr<int>(VERA2VEDA(ptr)).ptr(), ptr2, cnt));
		printf("veraLaunchKernel(%p, %i, %p, %p, %llu)\n", func, 0, VEDAptr<int>(VERA2VEDA(ptr)).ptr(), ptr2, cnt);

		CHECK(veraMemcpyAsync(host, ptr2, size, veraMemcpyDeviceToHost, 0));
		printf("veraMemcpyAsync(%p, %p, %llu, %i)\n", host, ptr2, size, 0);

		CHECK(veraDeviceSynchronize());
		printf("veraDeviceSynchronize()\n");

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != (cnt - 1 - i)) {
				printf("expected host[%i] to be %p but is %p\n", i, (cnt - 1 - i), host[i]);
				return 1;
			}
		}

		CHECK(veraModuleUnload(mod));
		printf("veraModuleUnload(%p)\n", mod);
		CHECK(veraFreeAsync(ptr, 0));
		printf("veraFreeAsync(%p, %i)\n", ptr, 0);
		CHECK(veraFreeAsync(ptr2, 0));
		printf("veraFreeAsync(%p, %i)\n", ptr, 0);

		free(host);
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	return 0;
}