#include <stdio.h>
#include <stdlib.h>
#include <veda.h>
#include <assert.h>

#define CHECK(err) check(err, __FILE__, __LINE__)

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char *name, *str;
		vedaGetErrorName	(err, &name);
		vedaGetErrorString	(err, &str);
		printf("%s: %s @ %s:%i\n", name, str, file, line);
		printf("\nTEST CASE ID: FT_VEDA_MODULE_XX:FAILED\n");
		assert(0);
		exit(1);
	}
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	VEDAcontext ctx;
	CHECK(vedaDevicePrimaryCtxRetain(&ctx, 0));
	CHECK(vedaCtxPushCurrent(ctx));

	VEDAmodule mod;
	printf("\nTEST CASE ID: FT_VEDA_MODULE_04:");
	VEDAresult ret = vedaModuleLoad(&mod, NULL);
	if(ret == VEDA_ERROR_INVALID_VALUE)
            printf("PASSED\n");
	else
	    printf("FAILED\n");

	printf("\nTEST CASE ID: FT_VEDA_MODULE_05:");
	ret = vedaModuleLoad(&mod, "");
	if(ret == VEDA_ERROR_INVALID_VALUE)
            printf("PASSED\n");
	else
	    printf("FAILED\n");

	printf("\nTEST CASE ID: FT_VEDA_MODULE_06:");
	ret = vedaModuleLoad(&mod, "./NotAvailable.vso");
	if(ret == VEDA_ERROR_MODULE_NOT_FOUND)
            printf("PASSED\n");
	else
	    printf("FAILED\n");

	printf("\nTEST CASE ID: FT_VEDA_MODULE_01:");
	CHECK(vedaModuleLoad(&mod, "./libveda_module_device.vso"));
	printf("PASSED\n");

	VEDAfunction func;
	printf("\nTEST CASE ID: FT_VEDA_MODULE_07:");
	ret = vedaModuleGetFunction(&func,mod , "notFoundFuncName");
	if(ret == VEDA_ERROR_FUNCTION_NOT_FOUND)
            printf("PASSED\n");
	else
	    printf("FAILED\n");

	printf("\nTEST CASE ID: FT_VEDA_MODULE_08:");
	ret = vedaModuleGetFunction(&func,mod , "");
	if(ret == VEDA_ERROR_FUNCTION_NOT_FOUND)
            printf("PASSED\n");
	else
	    printf("FAILED\n");

	printf("\nTEST CASE ID: FT_VEDA_MODULE_09:");
	ret = vedaModuleGetFunction(&func,mod , NULL);
	if(ret == VEDA_ERROR_INVALID_ARGS)
            printf("PASSED\n");
	else
	    printf("FAILED\n");

	printf("\nTEST CASE ID: FT_VEDA_MODULE_03:");
	CHECK(vedaModuleGetFunction(&func,mod , "hello_world"));
	printf("PASSED\n");

	VEDAargs args;
	CHECK(vedaArgsCreate(&args));
	CHECK(vedaCtxSynchronize());
	CHECK(vedaLaunchKernelEx(func, 0, args, 0, 0));
	
	CHECK(vedaCtxSynchronize());
	printf("\nTEST CASE ID: FT_VEDA_MODULE_02:");
	CHECK(vedaModuleUnload(mod));
	printf("PASSED\n");
	printf("\nTEST CASE ID: FT_VEDA_MODULE_10:");
	CHECK(vedaModuleLoad(&mod, "./libveda_module_device.vso"));
	CHECK(vedaModuleGetFunction(&func,mod , "hello_world2"));
	CHECK(vedaLaunchKernelEx(func, 0, args, 1, 0));
	printf("PASSED\n");
	CHECK(vedaCtxSynchronize());

        printf("\n# ------------------------------------- #\n");
        printf("# All Tests passed!                     #\n");
        printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());

	return 0;
}
