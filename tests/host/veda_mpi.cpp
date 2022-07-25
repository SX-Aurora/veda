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

void mpi_something(void* ptr, HMEMptr<double> vptr) {
	printf("VOID*: %p, VPTR: %p, VE: %p\n", ptr, (VEDAhmemptr)vptr, vptr.ptr());
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	VEDAcontext ctx;
	CHECK(vedaCtxCreate(&ctx, 0, 0));
	
	HMEMptr<char> mpi;
	CHECK(vedaHMemAlloc(&mpi, 128));

	mpi_something(mpi, mpi);
	mpi_something(mpi, mpi);

	auto copy = mpi;
	mpi_something(copy, copy);

	auto off = &mpi[25];
	mpi_something(off, off);
	
	auto off2 = mpi + 0xDEADBEEF;
	mpi_something(off2, off2);

	CHECK(vedaHMemFree(mpi));

	CHECK(vedaExit());
	return 0;
}