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

	for(int dev = 0; dev < devcnt; dev++) {
		VEDAcontext ctx;
		CHECK(vedaCtxCreate(&ctx, 0, dev));
                printf("\nTEST CASE ID: FT_VEDA_HMEM_20 vedaHMemPtr\n");
		VEDAhmemptr hptr;
		CHECK(vedaHMemAlloc(&hptr, 128));
		void* raw;
		CHECK(vedaHMemPtr(&raw, hptr));
		printf("HPTR: 0x%016llx, Raw: 0x%016llx\n", hptr, raw);

		CHECK(vedaHMemFree(hptr));
	}

	CHECK(vedaExit());
        printf("\n# ------------------------------------- #\n");
        printf("# All Tests passed!                     #\n");
        printf("# ------------------------------------- #\n\n");

        printf("vedaExit()\n");
	return 0;
}
