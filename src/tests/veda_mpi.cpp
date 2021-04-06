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

void mpi_something(void* ptr, VEDAdeviceptr vptr, VEDAmpiptr<float> mpi) {
	printf("VOID*: %p, VPTR: %p, VE: %p, HMEM: %p\n", ptr, vptr, vptr->ptr(), vptr->hmem());
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	VEDAcontext ctx;
	CHECK(vedaCtxCreate(&ctx, 0, 0));
	
	VEDAmpiptr<float> mpi;
	CHECK(vedaMemAlloc(mpi, 128));

	mpi_something(mpi, mpi, mpi);
	mpi_something(mpi, mpi, mpi);

	auto copy = mpi;
	mpi_something(copy, copy, copy);

	auto off = &mpi[25];
	mpi_something(off, off, off);
	
	auto off2 = mpi + 0xDEADBEEF / sizeof(float);
	mpi_something(off2, off2, off2);

	CHECK(vedaMemFree(mpi));

	CHECK(vedaExit());
	return 0;
}