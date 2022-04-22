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

		int size=16;
		VEDAptr<int> ptr;
		CHECK(vedaMemAllocAsync(&ptr, size, 0));
		printf("vedaMemAllocAsync(%p, %llu, %i)\n", &ptr, size, 0);

		VEDAmodule mod;
		const char* modName = "libveda_test.vso";
		CHECK(vedaModuleLoad(&mod, modName));
		printf("vedaModuleLoad(%p, \"%s\")\n", mod, modName);

		VEDAfunction func;
		const char* funcName = "ve_test";
		CHECK(vedaModuleGetFunction(&func, mod, funcName));
		printf("vedaModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);

		CHECK(vedaModuleUnload(mod));
		printf("vedaModuleUnload(%p)\n", mod);
		CHECK(vedaCtxDestroy(ctx));
		printf("vedaCtxDestroy(%p)\n", ctx);
		VEDAresult err = vedaMemFreeAsync(ptr, 0);
		if( err == VEDA_ERROR_UNKNOWN_CONTEXT)
			printf("Passed \n");
		else{
			printf("Failed \n");
			exit(1);
		}

	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}
