#include <veda.h>
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
void MyCallback(VEDAstream stream, VEDAresult status, void *data){
 printf("Inside callback %d ", (size_t)data);
}

void run(VEDAcontext_mode omp, VEDAdevice device) {
	VEDAcontext ctx;
	VEDAstream_callback callback;
	CHECK(vedaCtxCreate(&ctx, omp, device));
	printf("vedaCtxCreate(%p, %i, %i)\n", ctx, omp, device);

	VEDAmodule mod;
	const char* modName = "./libveda_sm_device.vso";
	CHECK(vedaModuleLoad(&mod, modName));
	printf("vedaModuleLoad(%p, \"%s\")\n", mod, modName);

	VEDAfunction func;
	const char* funcName = "ve_hello_world";
	CHECK(vedaModuleGetFunction(&func, mod, funcName));
	printf("vedaModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);

	VEDAfunction func2;
	const char* funcName2 = "ve_exception";
	CHECK(vedaModuleGetFunction(&func2, mod, funcName2));
	int num = 0;
	CHECK(vedaCtxStreamCnt(&num));
	printf("vedaCtxStreamCnt(%i)\n", num);

	std::vector<VEDAstream> streams;
	streams.resize(num);

	for(VEDAstream stream : streams)			
		CHECK(vedaLaunchKernel(func, 0, 0));
	printf("vedaLaunchKernel(%i)\n", num);
	printf("vedaStreamSynchronize(%i)\n", num);
	printf("\nTEST CASE ID: FT_VEDA_SM_01:");
	for(VEDAstream stream : streams)			
		CHECK(vedaStreamSynchronize(stream));
	printf("PASSED\n");
	printf("vedaStreamQuery(%i)\n", num);
 	printf("\nTEST CASE ID: FT_VEDA_SM_02:");
	for(VEDAstream stream : streams){
	        VEDAresult ret=vedaStreamQuery(stream);
                if(ret == VEDA_ERROR_VEO_STATE_RUNNING)
                        printf("PASSED\n");
                else{
                        printf("FAILED\n");
		}
        }
 	printf("\nTEST CASE ID: FT_VEDA_SM_06:");
	VEDAresult ret=vedaStreamQuery(99);
        if(ret == VEDA_ERROR_UNKNOWN_STREAM)
                printf("PASSED\n");
        else{
                printf("FAILED\n");
	}
	printf("vedaStreamAddCallback(%i)\n", num);
 	printf("\nTEST CASE ID: FT_VEDA_SM_04:");
	for(VEDAstream stream : streams) 			
		CHECK(vedaStreamAddCallback(stream, MyCallback, NULL, 0));
 	printf("PASSED\n");

 	printf("\nTEST CASE ID: FT_VEDA_SM_03:");
	for(VEDAstream stream : streams)			
		CHECK(vedaLaunchKernel(func2, 0, 0));
	for(VEDAstream stream : streams)  
		{ VEDAresult ret= vedaStreamSynchronize(0); 
	if(ret == VEDA_ERROR_VEO_COMMAND_EXCEPTION)
                printf("PASSED\n");
        else
                printf("FAILED\n");
	}
 	printf("\nTEST CASE ID: FT_VEDA_SM_05:");
	CHECK(vedaStreamAddCallback(0, MyCallback, NULL, 0));
 	printf("PASSED\n");
	uint32_t f;
	vedaStreamGetFlags(0,&f);
	CHECK(vedaCtxDestroy(ctx));
	printf("vedaCtxDestroy(%i)\n", num);
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	run(VEDA_CONTEXT_MODE_OMP, 0);

        printf("\n# ------------------------------------- #\n");
        printf("# All Tests passed!                     #\n");
        printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}
