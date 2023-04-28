#include <veda.h>
#include <vera.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#define CHECK(err) check(err, __FILE__, __LINE__)
#define SIZE 128

void check(VEDAresult err, const char* file, const int line) {
        if(err != VEDA_SUCCESS) {
                const char* name = 0;
                vedaGetErrorName(err, &name);
                printf("Error: %i %s @ %s (%i)\n", err, name, file, line);
                assert(false);
                exit(1);
        }
}

int main()
{
        VEDAhmemptr hmemptr_2d = 0;
        int w = 4, h = 4;
        size_t pitch_size = w;
        CHECK(vedaInit(0));
        VEDAcontext c1, c2;
	VEDAptr<int> ptr_2d = 0;
        CHECK(vedaCtxCreate(&c1, VEDA_CONTEXT_MODE_SCALAR, 0));
	printf("\nTEST CASE ID: FT_VEDA_HMEM_D2D_10 vedaHMemsetD2D128\n");
        CHECK(vedaHMemAlloc(&hmemptr_2d, SIZE *w*h));
	CHECK(vedaMemAlloc(&ptr_2d, SIZE));
	int64_t value1 = (int64_t)0x3456789012345678;
	CHECK(vedaMemsetD64(ptr_2d, 0x00, SIZE/sizeof(int64_t)));
	CHECK(vedaHMemsetD2D128(hmemptr_2d, pitch_size, value1, value1, w, h));
	CHECK(vedaHMemcpyXtoD(ptr_2d, hmemptr_2d, sizeof(int64_t) *w*h));

#ifndef NOCPP17
        printf("Host PTR: 0x%016llx, Device PTR: 0x%016llx\n",hmemptr_2d, ptr_2d);
#else
        printf("Host PTR: 0x%016llx, Device PTR: 0x%016llx\n",hmemptr_2d, &ptr_2d);
#endif

	printf("\nTEST CASE ID: FT_VEDA_HMEM_D2D_11 vedaHMemsetD2D128Async\n");
	int64_t value2 = (int64_t)0x123456789012345;
	CHECK(vedaMemsetD64(ptr_2d, 0x00, SIZE/sizeof(int64_t)));
	CHECK(vedaHMemsetD2D128Async(hmemptr_2d, pitch_size, value2, value2, w, h, 0));
	CHECK(vedaHMemcpyXtoD(ptr_2d, hmemptr_2d, sizeof(int64_t) *w*h));

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

