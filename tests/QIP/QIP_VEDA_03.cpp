#include <stdio.h>
#include <stdlib.h>
#include <veda.h>

#define VEDA(err) check(err, __FILE__, __LINE__)

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char *name, *str;
		vedaGetErrorName	(err, &name);
		vedaGetErrorString	(err, &str);
		printf("%s: %s @ %s:%i\n", name, str, file, line);
		exit(1);
	}
}

int main(int argc, char** argv) {
	printf("Hello World from Host!\n");

	VEDA(vedaInit(0));

	VEDAcontext ctx;
	VEDA(vedaDevicePrimaryCtxRetain(&ctx, 0));
	VEDA(vedaCtxPushCurrent(ctx));

	VEDAmodule mod;
	VEDA(vedaModuleLoad(&mod, "./libQIP_VEDA_03_ve.vso"));

	VEDAfunction func;
	VEDA(vedaModuleGetFunction(&func, mod, "test_HMEM"));

	VEDAargs args;
	VEDA(vedaArgsCreate(&args));
	int cnt=5;
        HMEMptr<int> vebuf;
        int ret = vedaHMemAlloc(&vebuf, sizeof(int)*cnt );
        if (ret != 0) {
            fprintf(stderr, "vedaHMemAlloc failed: %d", ret);
            exit(1);
        }

        VEDAhmemptr vptr = vebuf;
        ret = vedaArgsSetHMEM(args, 0, vptr);
        if (ret != VEDA_SUCCESS) {
            fprintf(stderr, "vedaArgsSetHMEM failed: %d", ret);
            exit(1);
        }
	vedaArgsSetI32( args, 1, cnt );
	VEDA(vedaLaunchKernel(func, 0, args));
	VEDAresult err = vedaCtxSynchronize();
	if(err == VEDA_ERROR_VEO_COMMAND_EXCEPTION)
	{
		printf("Passed\n");
	}
	printf("test_HMEM return \n");

	VEDA(vedaExit());

	return 0;
}
