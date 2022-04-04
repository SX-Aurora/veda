#include <veda.h>
#include <unistd.h>
#include "veda_ft.h"
#include <pthread.h>
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

int context_cleanup = 0,retval;
pthread_cond_t cond1=PTHREAD_COND_INITIALIZER;
pthread_mutex_t m1=PTHREAD_MUTEX_INITIALIZER;

void *t1(void *context)
{
	int cnt;
	pthread_mutex_lock(&m1);
	CHECK(vedaCtxSetCurrent(*((VEDAcontext*)context)));
	printf("Context is %p\n", *((VEDAcontext*)context));
	VEDAcontext current;
	context_cleanup = 1;
	vedaCtxStreamCnt(&cnt);
	pthread_cond_signal(&cond1);
	pthread_mutex_unlock(&m1);
	pthread_mutex_lock(&m1);
	while(context_cleanup != 2)
	{
		pthread_cond_wait(&cond1, &m1);
	}
	if( vedaCtxGetCurrent(&current) != VEDA_ERROR_CONTEXT_IS_DESTROYED)
	{
		printf("FT_VEDA_CONTEXT_10 failed\n");
		pthread_mutex_unlock(&m1);
		retval = -1;
		pthread_exit(&retval);
	}
	printf("TEST CASE ID: FT_VEDA_CONTEXT_11\n");
	printf("TEST CASE ID: FT_VEDA_CONTEXT_11\n");
	if( vedaCtxStreamCnt(&cnt) != VEDA_ERROR_CONTEXT_IS_DESTROYED)
	{
		printf("FT_VEDA_CONTEXT_11 failed\n");
		pthread_mutex_unlock(&m1);
		retval = -1;
		pthread_exit(&retval);
	}
       	
	if( vedaCtxDestroy(*((VEDAcontext*)context)) != VEDA_ERROR_CONTEXT_IS_DESTROYED)
	{
		printf("FT_VEDA_CONTEXT_11_1 failed\n");
		pthread_mutex_unlock(&m1);
		retval = -1;
		pthread_exit(&retval);
	}

	pthread_mutex_unlock(&m1);
	pthread_exit(&retval);

}
int main(int argc, char** argv) {
	pthread_t tid1;
	CHECK(vedaInit(0));

	// Get the VE node count through VEDA FT library 
	int devcnt,cnt, cores, *status;
	char num_of_omp_thread[256];
	CHECK(vedaDeviceGetCount(&devcnt));
	printf("\nTEST CASE ID: FT_VEDA_CONTEXT_01\n");
	printf("TEST CASE ID: FT_VEDA_CONTEXT_02\n");
	printf("TEST CASE ID: FT_VEDA_CONTEXT_03\n");
	printf("TEST CASE ID: FT_VEDA_CONTEXT_04\n");
    	VEDAcontext cont, current;
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaCtxCreate(&cont, VEDA_CONTEXT_MODE_OMP, dev));
		CHECK(vedaCtxGetCurrent(&current));
		if(cont != current)
		{
			printf("FT_VEDA_CONTEXT_04 failed\n");
			exit(0);
		}
		if( VEDA_ERROR_CANNOT_CREATE_CONTEXT != vedaCtxCreate(&cont, VEDA_CONTEXT_MODE_OMP, dev))
		{
			printf("FT_VEDA_CONTEXT_02 failed\n");
			exit(0);
		}
		CHECK(vedaCtxStreamCnt(&cnt));
		if(cnt != 1)
		{
			printf("FT_VEDA_CONTEXT_03 failed\n");
			exit(0);
		}
		CHECK(vedaCtxDestroy(cont));

	}
	printf("TEST CASE ID: FT_VEDA_CONTEXT_21\n");
	if(vedaCtxCreate(&cont, 2, 0) != VEDA_ERROR_INVALID_VALUE)
	{
		printf("FT_VEDA_CONTEXT_21 failed\n");
		exit(0);
	}
    	CHECK(vedaExit());
	printf("TEST CASE ID: FT_VEDA_CONTEXT_05\n");
      	CHECK(vedaInit(0));
      	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaDeviceGetAttribute(&cores,VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,dev));
      		CHECK(vedaCtxCreate(&cont, VEDA_CONTEXT_MODE_SCALAR, dev));
		CHECK(vedaCtxStreamCnt(&cnt));
		if(cnt != cores)
		{
			printf("FT_VEDA_CONTEXT_05 failed\n");
			exit(0);
		}
      }
      
      CHECK(vedaExit());
	printf("TEST CASE ID: FT_VEDA_CONTEXT_06\n");
	printf("TEST CASE ID: FT_VEDA_CONTEXT_07\n");
	printf("TEST CASE ID: FT_VEDA_CONTEXT_08\n");
	setenv("VE_OMP_NUM_THREADS","1", 1);
      	CHECK(vedaInit(0));
	if( vedaCtxGetCurrent(&current) != VEDA_ERROR_UNKNOWN_CONTEXT)
		{
			printf("FT_VEDA_CONTEXT_07 failed\n");
			exit(0);
		}
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaCtxCreate(&cont, VEDA_CONTEXT_MODE_SCALAR, dev));
		CHECK(vedaCtxStreamCnt(&cnt));
		if(cnt != 1)
		{
			printf("FT_VEDA_CONTEXT_06 failed\n");
			exit(0);
		}
		CHECK(vedaCtxDestroy(cont));
		if( vedaCtxGetCurrent(&current) != VEDA_ERROR_UNKNOWN_CONTEXT)
		{
			printf("FT_VEDA_CONTEXT_08 failed\n");
			exit(0);
		}
	}
      	CHECK(vedaExit());
      	CHECK(vedaInit(0));
	printf("TEST CASE ID: FT_VEDA_CONTEXT_09\n");
    	VEDAcontext c1, c2;
	CHECK(vedaDevicePrimaryCtxRetain(&c1, 0));
	CHECK(vedaCtxPushCurrent(c1));
	CHECK(vedaDevicePrimaryCtxRetain(&c2, 1));
	CHECK(vedaCtxPushCurrent(c2));
    	vedaCtxGetCurrent(&current);
	if(current != c2)
	{
		printf("FT_VEDA_CONTEXT_09 failed\n");
		exit(0);
	}
       	CHECK(vedaCtxDestroy(c2));
    	CHECK(vedaCtxGetCurrent(&current));
	if(current != c1)
	{
		printf("FT_VEDA_CONTEXT_09 failed\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_CONTEXT_10\n");
	printf("TEST CASE ID: FT_VEDA_CONTEXT_11\n");
	CHECK(vedaDevicePrimaryCtxRetain(&c2, 1));
	printf("Context is %p\n", c2);
	pthread_create(&tid1, NULL, &t1, (void *) &c2);
	pthread_mutex_lock(&m1);
	while(context_cleanup != 1)
	{
		pthread_cond_wait(&cond1, &m1);
	}
	printf("Destroying context\n");
       	CHECK(vedaCtxDestroy(c2));
	context_cleanup = 2;
	pthread_cond_signal(&cond1);
	pthread_mutex_unlock(&m1);
	pthread_join(tid1, (void **)&status);
	if(*status == -1)
	{
		printf("FT_VEDA_CONTEXT_10 failed\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_CONTEXT_12\n");
    	CHECK(vedaCtxGetCurrent(&current));
	if(current != c1)
	{
		printf("FT_VEDA_CONTEXT_12 failed\n");
		exit(0);
	}
	CHECK(vedaDevicePrimaryCtxRetain(&c2, 1));
	CHECK(vedaCtxSetCurrent(c2));
    	CHECK(vedaCtxGetCurrent(&current));
	if(current != c2)
	{
		printf("FT_VEDA_CONTEXT_12 failed\n");
		exit(0);
	}
	CHECK(vedaCtxSetCurrent(NULL));
	current=0;
	if(vedaCtxGetCurrent(&current) != VEDA_ERROR_UNKNOWN_CONTEXT)
	{
		printf("FT_VEDA_CONTEXT_12 failed\n");
		exit(0);
	}
	CHECK(vedaCtxSetCurrent(c2));
	CHECK(vedaCtxPopCurrent(&current));
	printf("TEST CASE ID: FT_VEDA_CONTEXT_13\n");
	CHECK(vedaCtxPushCurrent(c1));
	CHECK(vedaCtxPushCurrent(c2));
    	CHECK(vedaCtxGetCurrent(&current));
	if(current != c2)
	{
		printf("FT_VEDA_CONTEXT_13 failed\n");
		exit(0);
	}
	CHECK(vedaCtxPopCurrent(&current));
	if(current != c2)
	{
		printf("FT_VEDA_CONTEXT_13 failed\n");
		exit(0);
	}
    	CHECK(vedaCtxGetCurrent(&current));
	if(current != c1)
	{
		printf("FT_VEDA_CONTEXT_13 failed\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_CONTEXT_14\n");
	current=0;
    	CHECK(vedaCtxGetCurrent(&current));
	if(current != c1)
	{
		printf("FT_VEDA_CONTEXT_14 failed\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_CONTEXT_15\n");
	CHECK(vedaCtxPushCurrent(c2));
	current=0;
    	CHECK(vedaCtxGetCurrent(&current));
	if(current != c2)
	{
		printf("FT_VEDA_CONTEXT_15 failed\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_CONTEXT_16\n");
	VEDAdevice id;
	CHECK(vedaCtxGetDevice(&id));
	if(id != 1)
	{
		printf("FT_VEDA_CONTEXT_16 failed\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_CONTEXT_18\n");
	VEDAmodule mod;
	const char* modName = "libveda_test.vso";
	CHECK(vedaModuleLoad(&mod, modName));
	VEDAfunction func;
	const char* funcName = "ve_hello_world";
	CHECK(vedaModuleGetFunction(&func, mod, funcName));
	
	int num = 0;
	CHECK(vedaCtxStreamCnt(&num));
	printf("vedaCtxStreamCnt(%i)\n", num);

	std::vector<VEDAstream> streams;
	streams.resize(num);

	for(VEDAstream stream : streams)			CHECK(vedaLaunchKernel(func, stream, stream));
	CHECK(vedaCtxSynchronize());

	printf("TEST CASE ID: FT_VEDA_CONTEXT_19\n");
	CHECK(vedaModuleGetFunction(&func, mod, "ve_hello_world_fault"));
	VEDAargs args;
	CHECK(vedaArgsCreate(&args));
	std::vector<uint64_t> res;
	res.reserve(streams.size());
	for(VEDAstream stream : streams)
	{
		vedaArgsSetU64(args, 0, stream);
		CHECK(vedaLaunchKernelEx(func, stream, args, 0, &res.emplace_back()));
	}
	if( -1 != vedaCtxSynchronize())
	{
		printf("FT_VEDA_CONTEXT_19 failed\n");
		exit(0);
	}
	CHECK(vedaModuleUnload(mod));
	printf("TEST CASE ID: FT_VEDA_CONTEXT_20\n");
	uint32_t version;
	int aveo_id = 0;
	CHECK(vedaDeviceGetPhysicalId(&aveo_id, 1));
	CHECK(vedaCtxGetApiVersion(c2, &version));
	if(version != Get_VEDA_device_abi_version(aveo_id)){
		printf("FT_VEDA_CONTEXT_020 FAILED\n");
		exit(0);
	}
	printf("PASSED\n");


	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	//CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}
