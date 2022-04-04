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

		VEDAdeviceptr A, B, C;
		CHECK(vedaMemAllocAsync(&A, 0, 0));
		CHECK(vedaMemAllocAsync(&B, 1024, 0));
		CHECK(vedaMemAllocAsync(&C, 2048, 0));

		printf("BEFORE: ");
		CHECK(vedaMemReport());

		CHECK(vedaMemSwapAsync(A, B, 0));
		CHECK(vedaMemSwapAsync(A, C, 0));

		printf("AFTER: ");
		CHECK(vedaMemReport());

		CHECK(vedaMemFreeAsync(A, 0));
		CHECK(vedaMemFreeAsync(B, 0));
		CHECK(vedaMemFreeAsync(C, 0));

		CHECK(vedaCtxDestroy(ctx));
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}