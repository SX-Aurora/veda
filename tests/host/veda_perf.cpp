#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <chrono>

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

inline uint64_t time_ns(void) {
	auto duration = std::chrono::system_clock::now().time_since_epoch();
	return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
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

		VEDAmodule mod;
		const char* modName = "libveda_test.vso";
		CHECK(vedaModuleLoad(&mod, modName));

		VEDAfunction func;
		const char* funcName = "ve_empty";
		CHECK(vedaModuleGetFunction(&func, mod, funcName));

		constexpr size_t RUNS = 100000;
		printf("Running %llu kernel calls...\n", RUNS);
		auto start = time_ns();
		for(size_t i = 0; i < RUNS; i++)
			CHECK(vedaLaunchKernel(func, 0));
		CHECK(vedaCtxSynchronize());
		auto end = time_ns();
		auto total = end - start;
		auto total_ms = total / 1000000.0;
		auto avg = total / (double)RUNS / 1000.0;
		printf("Completed in %fms, %fµs/average\n", total_ms, avg);

		CHECK(vedaCtxDestroy(ctx));
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}