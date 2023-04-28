#include <veda.h>
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
void testInvalidArgsValue(VEDAargs& args);
void hostFunc(VEDAstream stream, VEDAresult status, void *data){
 printf("Inside hostFunc %d\n", (size_t)data);
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	int devcnt;
	CHECK(vedaDeviceGetCount(&devcnt));
	printf("vedaDeviceGetCount(%i)\n", devcnt);
	for(int dev = 0; dev < devcnt; dev++) {
		printf("\n# ------------------------------------- #\n");
		printf("# RUNNING TESTS ON %i                    #\n", dev);
		printf("# ------------------------------------- #\n\n");

		VEDAcontext ctx;
		CHECK(vedaCtxCreate(&ctx, 0, dev));
//		printf("vedaCtxCreate(%p, %i, %i)\n", ctx, 0, dev);

		char name[128];
		CHECK(vedaDeviceGetName(name, sizeof(name), dev));
//		printf("vedaDeviceGetName(\"%s\", %i, %i)\n", name, sizeof(name), dev);

		size_t cnt = 128;
		size_t size = cnt * sizeof(int);
		int* host = (int*)malloc(size);
		for(size_t i = 0; i < cnt; i++)
			host[i] = i;

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != i) {
				printf("expected host[%i] to be %p but is %p\n", i, i, host[i]);
				return 1;
			}
		}

		VEDAdeviceptr ptr;
		CHECK(vedaMemAllocAsync(&ptr, size, 0));
//		printf("vedaMemAllocAsync(%p, %llu, %i)\n", ptr, size, 0);
		VEDAmodule mod;
		const char* modName = "./libveda_ecm_device.vso";
		CHECK(vedaModuleLoad(&mod, modName));
//		printf("vedaModuleLoad(%p, \"%s\")\n", mod, modName);

		VEDAfunction func;
		const char* funcName = "ve_test";
		CHECK(vedaModuleGetFunction(&func, mod, funcName));
//		printf("vedaModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);

		VEDAfunction func2;
		const char* funcName2 = "ve_hello_world";
		CHECK(vedaModuleGetFunction(&func2, mod, funcName2));
		VEDAfunction func3;
		const char* funcName3 = "vedaSetArgs_dataType";
		CHECK(vedaModuleGetFunction(&func3, mod, funcName3));
		VEDAfunction func4;
		const char* funcName4 = "vedaSetArgs_float";
		CHECK(vedaModuleGetFunction(&func4, mod, funcName4));
		VEDAfunction func5;
		const char* funcName5 = "vedaSetArgs_double";
		CHECK(vedaModuleGetFunction(&func5, mod, funcName5));
		VEDAfunction func6;
		const char* funcName6 = "vedaArgsSetStack_test";
		CHECK(vedaModuleGetFunction(&func6, mod, funcName6));
		VEDAdeviceptr ptr2;
		CHECK(vedaMemAllocAsync(&ptr2, 0, 0));
//		printf("vedaMemAllocAsync(%p, %llu, %i)\n", ptr2, 0, 0);
		const int16_t cnt2=128;
		VEDAargs args=NULL;
		testInvalidArgsValue(args);
		CHECK(vedaArgsCreate(&args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_14:");
		CHECK(vedaArgsSetPtr(args, 0, ptr));
	 	printf("PASSED\n");
		printf("\nTEST CASE ID: FT_VEDA_ECM_15:");
		CHECK(vedaArgsSetVPtr(args, 1, ptr2));
	 	printf("PASSED\n");
		CHECK(vedaArgsSetU64(args, 2, cnt2));
		printf("\nTEST CASE ID: FT_VEDA_ECM_01:");
		CHECK(vedaLaunchKernel(func, 0, args));
	 	printf("PASSED\n");
		printf("\nTEST CASE ID: FT_VEDA_ECM_04:");
		CHECK(vedaArgsSetI8(args, 0,CHAR_MAX));
		CHECK(vedaLaunchKernel(func3, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_05:");
		CHECK(vedaArgsSetU8(args, 0,UCHAR_MAX));
		CHECK(vedaLaunchKernel(func3, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_06:");
		CHECK(vedaArgsSetI16(args, 0,SHRT_MAX));
		CHECK(vedaLaunchKernel(func3, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_07:");
		CHECK(vedaArgsSetU16(args, 0,USHRT_MAX));
		CHECK(vedaLaunchKernel(func3, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_08:");
		CHECK(vedaArgsSetI32(args, 0,INT_MAX));
		CHECK(vedaLaunchKernel(func3, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_09:");
		CHECK(vedaArgsSetU32(args, 0,UINT_MAX));
		CHECK(vedaLaunchKernel(func3, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_10:");
		CHECK(vedaArgsSetI64(args, 0,LONG_MAX));
		CHECK(vedaLaunchKernel(func3, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_11:");
		CHECK(vedaArgsSetU64(args, 0,ULONG_MAX));
		CHECK(vedaLaunchKernel(func3, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_12:");
		CHECK(vedaArgsSetF32(args, 0,FLT_MAX));
		CHECK(vedaLaunchKernel(func4, 0, args));
		printf("\nTEST CASE ID: FT_VEDA_ECM_13:");
		CHECK(vedaArgsSetF64(args, 0,DBL_MAX ));
		CHECK(vedaLaunchKernel(func5, 0, args));
//		printf("vedaLaunchKernel(%p, %i, %p, %p, %llu)\n", func, 0, ptr, ptr2, cnt);
//
//		CHECK(vedaLaunchKernel(func4, 0, args));

		printf("\nTEST CASE ID: FT_VEDA_ECM_02:");
		CHECK(vedaLaunchKernelEx(func2, 0, args, 1, 0));
//		printf("vedaLaunchKernelEx(%p, %i, %p, %p, %llu)\n", func, 0, ptr, ptr2, cnt);
	 	printf("PASSED\n");

//		vedaLaunchHostFunc		(VEDAstream stream, VEDAhost_function fn, void* userData);
		int i = 5;
		void* data = &i;
		printf("\nTEST CASE ID: FT_VEDA_ECM_03:");
		CHECK(vedaLaunchHostFunc(0, (VEDAhost_function)hostFunc, (void*)args)); 
	 	printf("PASSED\n");

		CHECK(vedaCtxSynchronize());
		printf("vedaCtxSynchronize()\n");

		CHECK(vedaMemcpyDtoHAsync(host, ptr2, size, 0));
		printf("vedaMemcpyDtoHAsync(%p, %p, %llu, %i)\n", host, ptr2, size, 0);

		CHECK(vedaCtxSynchronize());
		printf("vedaCtxSynchronize()\n");

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != (cnt - 1 - i)) {
				printf("expected host[%i] to be %p but is %p\n", i, (cnt - 1 - i), host[i]);
				return 1;
			}
		}
		const char* ver_str;
		vedaDriverGetVersion(&ver_str);
		printf("vedaDriverGetVersion=%s\n",ver_str);
		if( ver_str != NULL)
                printf("PASSED\n");
                else{
                       printf("FAILED\n");
                       exit(1);
                }
    		printf("\nTEST CASE ID: FT_VEDA_ECM_16:");

	#if 0 // DISABLED
		VEDAargs args2=NULL;
		CHECK(vedaArgsCreate(&args2));
		void *ptr4;
		VEDAptr<char> mpi;
		CHECK(vedaMemAlloc(&mpi, 128));
		void* vptr = mpi.hmem();
		VEDAresult ret = vedaArgsSetHMEM(args2, 0, vptr);
		ret = vedaArgsSetHMEM(args2, 0, vptr);
	       	if(ret != VEDA_ERROR_INVALID_VALUE)
   	 		printf("PASSED\n");
    		else
     			{printf("FAILED\n");exit(1);}
    
		int array[10];
		for(int i= 0; i < 10; i++)
			array[i]=i;
		printf("\nTEST CASE ID: FT_VEDA_ECM_17:");
	        CHECK(vedaArgsSetStack(args2, 0, array, VEDA_ARGS_INTENT_INOUT, sizeof(int)*10));
		CHECK(vedaArgsSetI8(args2, 1,10));
        	CHECK(vedaLaunchKernel(func6, 0, args2));
   	 	printf("PASSED\n");

		CHECK(vedaMemFree(mpi));
	#endif
		CHECK(vedaModuleUnload(mod));
//		printf("vedaModuleUnload(%p)\n", mod);
		CHECK(vedaMemFreeAsync(ptr, 0));
//		printf("vedaMemFree(%p, %i)\n", ptr, 0);
		CHECK(vedaMemFreeAsync(ptr2, 0));
//		printf("vedaMemFree(%p, %i)\n", ptr2, 0);
		CHECK(vedaCtxDestroy(ctx));
//		printf("vedaCtxDestroy(%p)\n", ctx);
		free(host);
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}

void testInvalidArgsValue(VEDAargs& args) {

    printf("\nTEST CASE ID: FT_VEDA_ECM_18:");
    VEDAresult ret = vedaArgsSetI8(args, 0,CHAR_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_19:");
    ret = vedaArgsSetU8(args, 0,UCHAR_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_20:");
    ret = vedaArgsSetI16(args, 0,SHRT_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_21:");
    ret = vedaArgsSetU16(args, 0,USHRT_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_22:");
    ret = vedaArgsSetI32(args, 0,INT_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_23:");
    ret = vedaArgsSetU32(args, 0,UINT_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_24:");
    ret = vedaArgsSetI64(args, 0,LONG_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_25:");
    ret = vedaArgsSetU64(args, 0,ULONG_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_26:");
    ret = vedaArgsSetF32(args, 0,FLT_MAX);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_27:");
    ret = vedaArgsSetF64(args, 0,DBL_MAX );
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    VEDAdeviceptr ptr,ptr2;
    printf("\nTEST CASE ID: FT_VEDA_ECM_28:");
    ret = vedaArgsSetPtr(args, 0, ptr);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_29:");
    ret = vedaArgsSetVPtr(args, 0, ptr2);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}

#if 0 // DISABLED
    printf("\nTEST CASE ID: FT_VEDA_ECM_30:");
    ret = vedaArgsSetHMEM(args, 0, ptr2);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}
#endif
    
    printf("\nTEST CASE ID: FT_VEDA_ECM_31:");
    ret = vedaArgsSetStack(args, 0, ptr2,VEDA_ARGS_INTENT_INOUT, 1);
    if(ret == VEDA_ERROR_INVALID_ARGS)
    	printf("PASSED\n");
    else
     	{printf("FAILED\n");exit(1);}

}
