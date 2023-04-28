#include <veda.h>
#include <vera.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>

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
        VEDAcontext cont;
        CHECK(vedaCtxCreate(&cont, VEDA_CONTEXT_MODE_SCALAR, 0));
	VEDAhmemptr hptr;
	CHECK(vedaHMemAlloc(&hptr, 128));
	VEDAmodule mod;
	printf("\nTEST CASE ID: FT_VEDA_HMEM_21 vedaHMemPtr\n\n");
	
	const char* modName = "./libve_hmem_ptr.vso";
	CHECK(vedaModuleLoad(&mod, modName));
	printf("vedaModuleLoad(%p, \"%s\")\n", mod, modName);

	VEDAfunction func;
	const char* funcName = "test_HMEMPTR";
	CHECK(vedaModuleGetFunction(&func, mod, funcName));
	printf("vedaModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);
	VEDAargs args;
	CHECK(vedaArgsCreate(&args));
	CHECK(vedaArgsSetHMEM(args, 0, hptr));
	printf("vedaLaunchKernel\n");
	CHECK(vedaLaunchKernel(func, 0, args));
	printf("vedaCtxSynchronize\n");
	CHECK(vedaCtxSynchronize());
	printf("vedaCtxDestroy\n");
	CHECK(vedaHMemFree(hptr));
	CHECK(vedaCtxDestroy(cont));
        CHECK(vedaExit());

	printf("\n# ------------------------------------- #\n");
        printf("# All Tests passed!                     #\n");
        printf("# ------------------------------------- #\n\n");

}
