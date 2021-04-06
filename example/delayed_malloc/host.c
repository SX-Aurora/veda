#include <stdio.h>
#include <stdlib.h>
#include <veda.h>

#define VEDA(err) check(err, __FILE__, __LINE__)

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char *name, *str;
		vedaGetErrorName	(err, &name);
		vedaGetErrorString	(err, &str);
		printf("%s: %s @ %s:%i\n", name, str, file, line);
		exit(1);
	}
}

int main(int argc, char** argv) {
	VEDA(vedaInit(0));

	VEDAcontext ctx;
	VEDA(vedaDevicePrimaryCtxRetain(&ctx, 0));
	VEDA(vedaCtxPushCurrent(ctx));

	VEDAmodule mod;
	VEDA(vedaModuleLoad(&mod, "libdelayed_malloc_device.vso"));

	VEDAfunction func;
	VEDA(vedaModuleGetFunction(&func, mod, "delayed_malloc"));

	char workload[] = "Test123";
	VEDAdeviceptr host_allocated = 0;
	VEDAdeviceptr device_allocated = 0;

	size_t size = sizeof(workload);
	printf("Host: %s\n", workload);
	VEDA(vedaMemAllocAsync(&host_allocated, size, 0));
	VEDA(vedaMemcpyHtoDAsync(host_allocated, workload, size, 0));
	VEDA(vedaMemAllocAsync(&device_allocated, 0, 0)); // will be allocated by the device

	VEDAargs args;
	VEDA(vedaArgsCreate(&args));
	VEDA(vedaArgsSetVPtr(args, 0, host_allocated));
	VEDA(vedaArgsSetVPtr(args, 1, device_allocated));
	VEDA(vedaArgsSetU64(args, 2, size));
	VEDA(vedaLaunchKernelEx(func, 0, args, 1));
	VEDA(vedaMemcpyDtoHAsync(workload, device_allocated, size, 0));
	VEDA(vedaMemFreeAsync(host_allocated, 0));
	VEDA(vedaMemFreeAsync(device_allocated, 0));	
	VEDA(vedaCtxSynchronize());
	printf("Host: %s\n", workload);
	VEDA(vedaExit());

	return 0;
}