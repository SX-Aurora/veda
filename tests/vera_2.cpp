#include <vera.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <chrono>

#define NOW() std::chrono::high_resolution_clock::now()
#define time(start, end) (std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/1000000.0)
#define CHECK(err) check(err, __FILE__, __LINE__)

void check(veraError_t err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		printf("Error: %i %s @ %s (%i)\n", err, veraGetErrorName(err), file, line);
		assert(false);
		exit(1);
	}
}

int main(int argc, char** argv) {
	size_t cnt = 1024*1024*10;
	size_t size = cnt * sizeof(size);

	int* host = (int*)malloc(size);
	for(size_t i = 0; i < cnt; i++)
		host[i] = i;

	auto start = NOW(), end = NOW();

	// Copy to VE0
	CHECK(veraSetDevice(0));
	void* ptr0;
	CHECK(veraMallocAsync(&ptr0, size, 0));
	for(int i = 0; i < 10; i++) {
		start = NOW();
		CHECK(veraMemcpyAsync(ptr0, host, size, veraMemcpyHostToDevice, 0));
		CHECK(veraDeviceSynchronize());
		end = NOW();
		printf("host >> v0 = %fms\n", time(start, end));
	}

	for(int i = 0; i < 10; i++) {
		start = NOW();
		CHECK(veraMemcpyAsync(host, ptr0, size, veraMemcpyDeviceToHost, 0));
		CHECK(veraDeviceSynchronize());
		end = NOW();
		printf("host << v0 = %fms\n", time(start, end));
	}
	memset(host, 0, size);

	// Copy from VE0 to VE1
	CHECK(veraSetDevice(1));
	void* ptr1;
	CHECK(veraMallocAsync(&ptr1, size, 0));
	CHECK(veraDeviceSynchronize());

	for(int i = 0; i < 10; i++) {
		start = NOW();
		CHECK(veraMemcpyAsync(ptr1, ptr0, size, veraMemcpyDeviceToDevice, 0));
		CHECK(veraDeviceSynchronize());
		end = NOW();
		printf("ve0 >> ve1 = %fms\n", time(start, end));
	}

	for(int i = 0; i < 10; i++) {
		start = NOW();
		CHECK(veraMemcpyAsync(host, ptr1, size, veraMemcpyDeviceToHost, 0));
		CHECK(veraDeviceSynchronize());
		end = NOW();
		printf("ve1 >> host = %fms\n", time(start, end));
	}

	// Check results
	for(size_t i = 0; i < cnt; i++)
		if(host[i] != i)
			return 1;
	
	start = NOW();
	CHECK(veraFreeAsync(ptr1, 0));	
	CHECK(veraSetDevice(0));
	CHECK(veraFreeAsync(ptr0, 0));
	end = NOW();
	printf("free = %fms\n", time(start, end));
	free(host);

	printf("COMPLETE!\n");
	return 0;
}