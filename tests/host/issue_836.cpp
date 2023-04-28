#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <veda.h>

#define CHECK(err) check(err, __FILE__, __LINE__)

void check(VEDAresult err, const char* file, const int line)
{
	if (err != VEDA_SUCCESS) {
		const char *name, *str;
		vedaGetErrorName(err, &name);
		vedaGetErrorString(err, &str);
		printf("%s: %s @ %s:%i\n", name, str, file, line);
		exit(1);
	}
}

int main(int argc, char** argv)
{
	size_t n = 256;
	VEDAargs args;

	CHECK(vedaInit(0));

	VEDAcontext ctx;
	CHECK(vedaDevicePrimaryCtxRetain(&ctx, 0));
	CHECK(vedaCtxPushCurrent(ctx));

	VEDAmodule mod;
	CHECK(vedaModuleLoad(&mod, "libissue_836.vso"));

	VEDAfunction fun_kernel1;
	CHECK(vedaModuleGetFunction(&fun_kernel1, mod, "fun_kernel1"));
#define MODE 1
#if MODE == 0
	VEDAfunction fun_alloc_init;
	CHECK(vedaModuleGetFunction(&fun_alloc_init, mod, "fun_alloc_init"));
#endif

	VEDAdeviceptr vptr2;
	CHECK(vedaMemAllocAsync(&vptr2, sizeof(double) * n, 0));

#if MODE == 1
	VEDAfunction fun_alloc_init;
	CHECK(vedaModuleGetFunction(&fun_alloc_init, mod, "fun_alloc_init"));
#endif

	CHECK(vedaArgsCreate(&args));
	CHECK(vedaArgsSetI64(args, 0, n));
	CHECK(vedaArgsSetVPtr(args, 1, vptr2));
	CHECK(vedaLaunchKernel(fun_alloc_init, 0, args));
	CHECK(vedaArgsDestroy(args));

	CHECK(vedaArgsCreate(&args));
	CHECK(vedaArgsSetF64(args, 0, 3.14));
	CHECK(vedaArgsSetI64(args, 1, n));
	CHECK(vedaArgsSetPtr(args, 2, vptr2));
	CHECK(vedaLaunchKernel(fun_kernel1, 0, args));
	CHECK(vedaArgsDestroy(args));
	CHECK(vedaCtxSynchronize());
	{ double *ptr; vedaMemPtr((void **) &ptr, vptr2); printf("VH: vedaMemAllocAsync vptr=%p\n", ptr);}

	CHECK(vedaMemFree(vptr2));

	CHECK(vedaCtxDestroy(ctx));
	CHECK(vedaExit());

	return 0;
}
