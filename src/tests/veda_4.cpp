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

		VEDAdeviceptr vptr;
		CHECK(vedaMemAlloc(&vptr, 128));

		auto res  = vptr->ptrSize();
		auto hmem = vptr->hmem();

		printf("VPTR: 0x%016llx, Raw: 0x%016llx, HMEM: 0x%016llx, Size: %lluB\n", vptr, res.ptr, hmem, res.size);

		CHECK(vedaMemFree(vptr));
	}

	CHECK(vedaExit());
	return 0;
}