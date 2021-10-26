#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <cfloat>
#include <cmath>
#include <algorithm>

#define CHECK(err) check(err, __FILE__, __LINE__)

uint64_t time_ns(void) {
	auto duration = std::chrono::system_clock::now().time_since_epoch();
	return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char* name = 0;
		vedaGetErrorName(err, &name);
		printf("Error: %i %s @ %s (%i)\n", err, name, file, line);
		assert(false);
		exit(1);
	}
}


#define STR(...) __STR(__VA_ARGS__)
#define __STR(...) #__VA_ARGS__

#define RUNS 100
#define RUN(FUNC, cnt, ...) {\
	double min = DBL_MAX;\
	double max = 0;\
	double sum = 0;\
	for(int i = 0; i < RUNS; i++){\
		auto start = time_ns();\
		CHECK(FUNC(ptr, __VA_ARGS__, cnt));\
		auto end = time_ns();\
		auto time = (end - start) / 1000.0;\
		min = std::min(min, time);\
		max = std::max(max, time);\
		sum += time;\
	}\
	printf("%-15sbytes: %8.2fMB, cnt: %10llu, min: %8.2f ms, avg: %8.2f, max: %8.2f ms\n", STR(FUNC), bytes/1024.0/1024.0, cnt, min, sum/RUNS, max);\
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	int devcnt;
	CHECK(vedaDeviceGetCount(&devcnt));

	const char* ver = 0;
	CHECK(vedaGetVersion(&ver));
	printf("VERSION: %s\n", ver);

	for(int dev = 0; dev < devcnt; dev++) {
		printf("\n# ------------------------------------- #\n");
		printf("# RUNNING TESTS ON %i                    #\n", dev);
		printf("# ------------------------------------- #\n\n");

		VEDAcontext ctx;
		CHECK(vedaCtxCreate(&ctx, 0, dev));
		
		// Correctness Tests -------------------------------------------
		const auto x64 = 0x01234567890ABCDEllu;
		const auto y64 = 0xEDCBA09876543210llu;

		const auto x8  = ((uint8_t*) &x64)[0];
		const auto x16 = ((uint16_t*)&x64)[0];
		const auto x32 = ((uint32_t*)&x64)[0];

	#if 1
		VEDAmodule mod;
		const char* modName = "libveda_test.vso";
		CHECK(vedaModuleLoad(&mod, modName));

		VEDAfunction check_d8, check_d16, check_d32, check_d64, check_d128;
		CHECK(vedaModuleGetFunction(&check_d8,   mod, "ve_test_memset_d8"));
		CHECK(vedaModuleGetFunction(&check_d16,  mod, "ve_test_memset_d16"));
		CHECK(vedaModuleGetFunction(&check_d32,  mod, "ve_test_memset_d32"));
		CHECK(vedaModuleGetFunction(&check_d64,  mod, "ve_test_memset_d64"));
		CHECK(vedaModuleGetFunction(&check_d128, mod, "ve_test_memset_d128"));

		size_t cnts[]    = {1, 5, 300, 23823, 34949, 231230};
		size_t offsets[] = {0, 0, 1,   3,     7,     231};

		VEDAdeviceptr d8, d16, d32, d64, d128;
		for(size_t i = 0; i < sizeof(cnts) / sizeof(size_t); i++) {
			CHECK(vedaMemAllocAsync(&d8,   cnts[i] * sizeof(uint8_t),      0));
			CHECK(vedaMemAllocAsync(&d16,  cnts[i] * sizeof(uint16_t),     0));
			CHECK(vedaMemAllocAsync(&d32,  cnts[i] * sizeof(uint32_t),     0));
			CHECK(vedaMemAllocAsync(&d64,  cnts[i] * sizeof(uint64_t),     0));
			CHECK(vedaMemAllocAsync(&d128, cnts[i] * sizeof(uint64_t) * 2, 0));

			size_t cnt = cnts[i] - offsets[i];
			auto n8   = d8   + offsets[i] * sizeof(uint8_t);
			auto n16  = d16  + offsets[i] * sizeof(uint16_t);
			auto n32  = d32  + offsets[i] * sizeof(uint32_t);
			auto n64  = d64  + offsets[i] * sizeof(uint64_t);
			auto n128 = d128 + offsets[i] * sizeof(uint64_t) * 2;
			
			CHECK(vedaMemsetD8Async		(             n8, x8, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d8, 0, n8, x8, cnt   ));

			CHECK(vedaMemsetD8Async		(             n8, (int8_t)x8, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d8, 0, n8, (int8_t)x8, cnt   ));
		
			CHECK(vedaMemsetD16Async	(              n16, x16, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d16, 0, n16, x16, cnt   ));

			CHECK(vedaMemsetD16Async	(              n16, (int16_t)x16, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d16, 0, n16, (int16_t)x16, cnt   ));

			CHECK(vedaMemsetD32Async	(              n32,  x32, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d32, 0, n32,  x32, cnt   ));

			CHECK(vedaMemsetD32Async	(              n32, (int32_t)x32, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d32, 0, n32, (int32_t)x32, cnt   ));

			CHECK(vedaMemsetD32Async	(              n32, (float)x32, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d32, 0, n32, (float)x32, cnt   ));

			CHECK(vedaMemsetD64Async	(              n64, x64, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d64, 0, n64, x64, cnt));

			CHECK(vedaMemsetD64Async	(              n64, (int64_t)x64, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d64, 0, n64, (int64_t)x64, cnt   ));

			CHECK(vedaMemsetD64Async	(              n64, (double)x64, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d64, 0, n64, (double)x64, cnt   ));

			CHECK(vedaMemsetD128Async	(              n128, x64, y64, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d128,0, n128, x64, y64, cnt   ));

			CHECK(vedaMemsetD128Async	(              n128, (int64_t)x64, (int64_t)y64, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d128,0, n128, (int64_t)x64, (int64_t)y64, cnt   ));

			CHECK(vedaMemsetD128Async	(              n128, (double)x64, (double)y64, cnt, 0));
			CHECK(vedaLaunchKernel		(check_d128,0, n128, (double)x64, (double)y64, cnt   ));


			CHECK(vedaMemFreeAsync(d8,   0));
			CHECK(vedaMemFreeAsync(d16,  0));
			CHECK(vedaMemFreeAsync(d32,  0));
			CHECK(vedaMemFreeAsync(d64,  0));
			CHECK(vedaMemFreeAsync(d128, 0));
		}
	#endif
	
		// Memcpy Tests ------------------------------------------------
		const double MB[] = {0.1, 0.2, 0.4, 0.8, 1, 2, 3, 4, 8, 16, 32, 64, 128, 256};

		for(size_t i = 0; i < sizeof(MB)/sizeof(size_t); i++) {
			auto bytes = size_t(MB[i] * 1024 * 1024);
			VEDAdeviceptr A, B;
			CHECK(vedaMemAllocAsync(&A, bytes, 0));
			CHECK(vedaMemAllocAsync(&B, bytes, 0));

			double min = DBL_MAX;
			double max = 0;
			double sum = 0;
			for(int i = 0; i < RUNS; i++){
				auto start = time_ns();
				CHECK(vedaMemcpyDtoD(A, B, bytes));
				auto end = time_ns();
				auto time = (end - start) / 1000.0;
				min = std::min(min, time);
				max = std::max(max, time);
				sum += time;
			}
			printf("%-15sbytes: %8.2fMB, cnt: %10llu, min: %8.2f ms, avg: %8.2f, max: %8.2f ms\n", "vedaMemcpyDtoD", bytes/1024.0/1024.0, bytes, min, sum/RUNS, max);\

			CHECK(vedaMemFreeAsync(A, 0));
			CHECK(vedaMemFreeAsync(B, 0));
		}

		// Memset Tests ------------------------------------------------
		for(size_t i = 0; i < sizeof(MB)/sizeof(size_t); i++) {
			auto bytes = size_t(MB[i] * 1024 * 1024);
			VEDAdeviceptr ptr;
			CHECK(vedaMemAllocAsync(&ptr, bytes, 0));

			RUN(vedaMemsetD8,   bytes / sizeof(uint8_t),      x8      )
			RUN(vedaMemsetD16,  bytes / sizeof(uint16_t),     x16     )
			RUN(vedaMemsetD32,  bytes / sizeof(uint32_t),     x32     )
			RUN(vedaMemsetD64,  bytes / sizeof(uint64_t),     x64     )
			RUN(vedaMemsetD128, bytes / sizeof(uint64_t) / 2, x64, y64)

			CHECK(vedaMemFreeAsync(ptr, 0));
		}

		CHECK(vedaCtxDestroy(ctx));
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	return 0;
}