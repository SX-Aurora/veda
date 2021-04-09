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

void mpi_something(void* ptr, VEDAptr<double> vptr) {
	printf("VOID*: %p, VPTR: %p, VE: %p, HMEM: %p\n", ptr, (VEDAdeviceptr)vptr, vptr.ptr(), vptr.hmem());
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	VEDAcontext ctx;
	CHECK(vedaCtxCreate(&ctx, 0, 0));
	
	VEDAptr<char> mpi;
	CHECK(vedaMemAlloc(&mpi, 128));

	mpi_something(mpi, mpi);
	mpi_something(mpi, mpi);

	auto copy = mpi;
	mpi_something(copy, copy);

	auto off = &mpi[25];
	mpi_something(off, off);
	
	auto off2 = mpi + 0xDEADBEEF;
	mpi_something(off2, off2);

	CHECK(vedaMemFree(mpi));

	CHECK(vedaExit());
	return 0;
}