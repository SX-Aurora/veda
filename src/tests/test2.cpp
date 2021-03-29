#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <chrono>

#define NOW() std::chrono::high_resolution_clock::now()
#define time(start, end) (std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/1000000.0)
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

	auto start = NOW();
	VEDAcontext dev0;
	CHECK(vedaCtxCreate(&dev0, 0, 0));
	auto end = NOW();
	printf("vedaCtxCreate(%p, %i, %i) = %fms\n", dev0, 0, 0, time(start, end));

	start = NOW();
	VEDAcontext dev1;
	CHECK(vedaCtxCreate(&dev1, 0, 1));
	end = NOW();
	printf("vedaCtxCreate(%p, %i, %i) = %fms\n", dev1, 0, 1, time(start, end));

	size_t cnt = 1024*1024*10;
	size_t size = cnt * sizeof(size);

	int* host = (int*)malloc(size);
	for(size_t i = 0; i < cnt; i++)
		host[i] = i;

	// Copy to VE0
	CHECK(vedaCtxSetCurrent(dev0));
	VEDAdeviceptr ptr0;
	CHECK(vedaMemAllocAsync(&ptr0, size, 0));
	for(int i = 0; i < 10; i++) {
		start = NOW();
		CHECK(vedaMemcpyHtoDAsync(ptr0, host, size, 0));
		CHECK(vedaCtxSynchronize());
		end = NOW();
		printf("host >> v0 = %fms\n", time(start, end));
	}

	for(int i = 0; i < 10; i++) {
		start = NOW();
		CHECK(vedaMemcpyDtoHAsync(host, ptr0, size, 0));
		CHECK(vedaCtxSynchronize());
		end = NOW();
		printf("host << v0 = %fms\n", time(start, end));
	}
	memset(host, 0, size);

	// Copy from VE0 to VE1
	CHECK(vedaCtxSetCurrent(dev1));
	VEDAdeviceptr ptr1;
	CHECK(vedaMemAllocAsync(&ptr1, size, 0));
	CHECK(vedaCtxSynchronize());

	for(int i = 0; i < 10; i++) {
		start = NOW();
		CHECK(vedaMemcpyDtoDAsync(ptr1, ptr0, size, 0));
		CHECK(vedaCtxSynchronize());
		end = NOW();
		printf("ve0 >> ve1 = %fms\n", time(start, end));
	}

	for(int i = 0; i < 10; i++) {
		start = NOW();
		CHECK(vedaMemcpyDtoHAsync(host, ptr1, size, 0));
		CHECK(vedaCtxSynchronize());
		end = NOW();
		printf("ve1 >> host = %fms\n", time(start, end));
	}

	// Check results
	for(size_t i = 0; i < cnt; i++)
		if(host[i] != i)
			return 1;
	
	start = NOW();
	CHECK(vedaMemFreeAsync(ptr1, 0));	
	CHECK(vedaCtxSetCurrent(dev0));
	CHECK(vedaMemFreeAsync(ptr0, 0));
	end = NOW();
	printf("fre = %fms\n", time(start, end));

	start = NOW();
	CHECK(vedaCtxDestroy(dev0));
	CHECK(vedaCtxDestroy(dev1));
	end = NOW();
	printf("destroy = %fms\n", time(start, end));

	free(host);

	CHECK(vedaExit());
	printf("COMPLETE!\n");
	return 0;
}