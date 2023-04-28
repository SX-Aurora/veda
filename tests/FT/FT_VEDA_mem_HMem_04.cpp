#include <veda.h>
#include <unistd.h>
#include "veda_ft.h"
#include <pthread.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>

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
	int devcnt = 0;
	CHECK(vedaDeviceGetCount(&devcnt));
    	VEDAcontext c1;
	CHECK(vedaCtxCreate(&c1, VEDA_CONTEXT_MODE_SCALAR, 0));
	VEDAhmemptr hmemptr_2d = 0;
	size_t pitch_size = 0, size_1d =128;
	int w = 4, h = 4;
	pitch_size = w;

        printf("\nTEST CASE ID: FT_VEDA_HMEM_D2D_06 vedaHMemsetD2D32\n");	
        CHECK(vedaHMemAlloc(&hmemptr_2d, size_1d *w*h));
        if(hmemptr_2d == 0)
        {
                printf("TEST CASE ID: FT_VEDA_HMEM_D2D_06 failed\n");
                exit(0);
	}

        VEDAptr<int> ptr_2d = 0;
        CHECK(vedaMemAlloc(&ptr_2d, size_1d));
	if(ptr_2d.size() != 128)
        {
                printf("TEST CASE ID: FT_VEDA_HMEM_D2D_06 failed\n");
                exit(0);
        }
	
	int32_t value = (int32_t)0x12345678u;
	CHECK(vedaMemsetD32(ptr_2d, 0x00, size_1d/sizeof(int32_t)));
	CHECK(vedaHMemsetD2D32(hmemptr_2d, pitch_size, value, w, h));
	CHECK(vedaHMemcpyXtoD(ptr_2d, hmemptr_2d, sizeof(int32_t) *w*h));

#ifndef NOCPP17
        printf("Host PTR: 0x%016llx, Device PTR: 0x%016llx\n",hmemptr_2d, ptr_2d);
#else
        printf("Host PTR: 0x%016llx, Device PTR: 0x%016llx\n",hmemptr_2d, &ptr_2d);
#endif
        printf("\nTEST CASE ID: FT_VEDA_HMEM_D2D_07 vedaHMemsetD2D32Async\n");	
	int32_t value1 = (int32_t)0x12344321u;
        CHECK(vedaMemsetD32(ptr_2d, 0x00, size_1d/sizeof(int32_t)));	
        CHECK(vedaHMemsetD2D32Async(hmemptr_2d, pitch_size, value1, w, h,0));
	CHECK(vedaHMemcpyXtoD(ptr_2d, hmemptr_2d, sizeof(int32_t) *w*h));

#ifndef NOCPP17
        printf("Host PTR: 0x%016llx, Device PTR: 0x%016llx\n",hmemptr_2d, ptr_2d);
#else
        printf("Host PTR: 0x%016llx, Device PTR: 0x%016llx\n",hmemptr_2d, &ptr_2d);
#endif

	CHECK(vedaHMemFree(hmemptr_2d));
        CHECK(vedaMemFree(ptr_2d));
        CHECK(vedaCtxDestroy(c1));
	CHECK(vedaExit());
	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	printf("vedaExit()\n");
	return 0;
}
