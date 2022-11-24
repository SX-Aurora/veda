#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <string_view>
#include <typeinfo>

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

inline uint64_t time_ns(void) {
	auto duration = std::chrono::system_clock::now().time_since_epoch();
	return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

static void callback(VEDAprofiler_data* data, const int enter) {
	auto end = time_ns();
	
	const char* tname;
	CHECK(vedaProfilerTypeName(data->type, &tname));
	if(data->device_id == -1)	printf("[host  ] ");
	else				printf("[ve:%i:%i] ", data->device_id, data->stream_id);
	printf("%-6s %-19s ", enter ? "Enter:" : "Exit:", tname);
	if(data->req_id == -1)	printf("      ");
	else			printf("#%3llu, ", data->req_id);

	if(!enter) {
		auto start	= *(uint64_t*)&data->user_data;
		auto ns 	= end - start;
		auto ms		= ns / (1024.0*1024.0);
		
		printf("time: %.3fms", ms);
		
		switch(data->type) {
			case VEDA_PROFILER_MEM_CPY_HTOD:
			case VEDA_PROFILER_MEM_CPY_DTOH:
			case VEDA_PROFILER_HMEM_CPY: {
				auto args	= (const VEDAprofiler_vedaMemcpy*)data;
				auto GB		= args->bytes	/(1024.0*1024.0*1024.0);
				auto s		= ns		/(1000.0*1000.0*1000.0);
				auto GBs	= GB/s;
				printf(", %p -> %p, %.2fGB/s", args->src, args->dst, GBs);
			} break;

			case VEDA_PROFILER_MEM_ALLOC:
			case VEDA_PROFILER_HMEM_ALLOC: {
				auto args = (const VEDAprofiler_vedaMemAlloc*)data;
				printf(", malloc(%lluB)", args->bytes);
			} break;

			case VEDA_PROFILER_MEM_FREE:
			case VEDA_PROFILER_HMEM_FREE: {
				auto args = (const VEDAprofiler_vedaMemFree*)data;
				printf(", %p", args->ptr);
			} break;
			
			case VEDA_PROFILER_LAUNCH_KERNEL: {
				auto args = (const VEDAprofiler_vedaLaunchKernel*)data;
				printf(", kernel: %s, func: %p", args->kernel, args->func);
			} break;
		
			case VEDA_PROFILER_LAUNCH_HOST: {
				auto args = (const VEDAprofiler_vedaLaunchHostFunc*)data;
				printf(", func: %p", args->func);
			} break;
		}
	}
	
	printf("\n");

	// start timer AFTER printf
	*((uint64_t*)&data->user_data) = time_ns();
}

static uint64_t some_host_func(void*) {
	//printf("some_host_func...\n");
	return 0;
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));
	CHECK(vedaProfilerSetCallback(&callback));
	CHECK(vedaProfilerSetCallback(0));
	CHECK(vedaProfilerSetCallback(&callback));

	int devcnt;
	CHECK(vedaDeviceGetCount(&devcnt));
	printf("vedaDeviceGetCount(%i)\n", devcnt);

	for(int dev = 0; dev < devcnt; dev++) {
		printf("\n# ------------------------------------- #\n");
		printf("# RUNNING TESTS ON %i                    #\n", dev);
		printf("# ------------------------------------- #\n\n");

		VEDAcontext ctx;
		CHECK(vedaCtxCreate(&ctx, 0, dev));

		auto cnt	= 209715200;
		auto size	= sizeof(int32_t) * cnt;
		auto host 	= malloc(size);

		VEDAptr<int> ptr;
		CHECK(vedaMemAllocAsync(&ptr, size, 0));
		CHECK(vedaMemcpyHtoDAsync(ptr, host, size, 0));
		CHECK(vedaMemsetD32Async(ptr, 0xDEADBEEF, cnt, 0));
		CHECK(vedaMemcpyDtoHAsync(host, ptr, size, 0));
		CHECK(vedaLaunchHostFunc(0, &some_host_func, 0));

		VEDAmodule mod;
		const char* modName = "libveda_test.vso";
		CHECK(vedaModuleLoad(&mod, modName));

		VEDAfunction func;
		const char* funcName = "ve_test";
		CHECK(vedaModuleGetFunction(&func, mod, funcName));

		VEDAdeviceptr ptr2;
		CHECK(vedaMemAllocAsync(&ptr2, 0, 0));

		uint64_t res = 0xDEADBEEF0C0FFEE0llu;
		CHECK(vedaLaunchKernelEx(func, 0, &res, ptr.ptr(), ptr2, cnt));

		CHECK(vedaMemFreeAsync(ptr, 0));
		CHECK(vedaMemFreeAsync(ptr2, 0));

		CHECK(vedaCtxSynchronize());

		VEDAhmemptr hmem;
		CHECK(vedaHMemAlloc(&hmem, size));
		CHECK(vedaHMemcpy(hmem, host, size));
		CHECK(vedaHMemcpy(host, hmem, size));
		CHECK(vedaHMemcpy(host, host, size));
		CHECK(vedaHMemFree(hmem));

		CHECK(vedaCtxDestroy(ctx));

		free(host);
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}