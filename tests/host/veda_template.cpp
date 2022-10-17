#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <cfloat>
#include <cmath>
#include <algorithm>

#include <veda/api_cpp_template.h>

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

		VEDAfunction func;
		VEDAmodule mod;
		vedaModuleGetCPPFunction::Template<float, float, int>::Return<void*>::Args<void*, int, float, bool, void*, void*, const int*, const int*, const void*, const void*>(&func, mod, "test");
		// _Z4testIffiEPvS0_ifbS0_S0_PKiS2_PKvS4_
		// _Z4testIffiEPvS0_ifbS0_S0_PKiS2_PKvS4_
		vedaModuleGetCPPFunction::Template<void*, int, bool*>::Return<void*>::Args<void*, int, float, bool, void*, void*, const int*, const int*, const void*, const void*>(&func, mod, "test");

		CHECK(vedaCtxDestroy(ctx));
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	return 0;
}