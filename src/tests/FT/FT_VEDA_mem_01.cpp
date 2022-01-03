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

	printf("\nTEST CASE ID: FT_VEDA_MEM_01\n");
    	VEDAcontext c1, c2;
	CHECK(vedaCtxCreate(&c1, VEDA_CONTEXT_MODE_SCALAR, 0));
	int8_t *h8, *host_2d;
	int32_t *h32;
	int64_t *h64;
	int16_t *host, *h16;
	VEDAptr<char> ptr_2d = 0;
	size_t pitch_size = 0, size_1d = 64,each_stream_res=0;
	int w = 4, h = 4, stream_cnt=0,device_id=-1;
	char *raw_addr=NULL;
	
	host = (int16_t*)malloc(64);


	VEDAptr<char> ptr = 0;
	CHECK(vedaMemAlloc(&ptr, size_1d));
	if(ptr.size() != 64)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_01 failed\n");
		exit(0);
	}
	CHECK(vedaMemPtrSize((void**)&raw_addr, &pitch_size, ptr));
	if(raw_addr == NULL)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_01 failed\n");
		exit(0);
	}
	printf("\nTEST CASE ID: FT_VEDA_MEM_02\n");
	CHECK(vedaMemAllocPitch(&ptr_2d, &pitch_size, sizeof(char)*w, h, sizeof(char)));
	if(pitch_size != sizeof(char)*w)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_02 failed\n");
		exit(0);
	}
	size_t mem_size=0;
	CHECK(vedaMemSize(&mem_size, ptr_2d));
	if(mem_size != w*h*sizeof(char))
	{
		printf("TEST CASE ID: FT_VEDA_MEM_02 failed\n");
		exit(0);
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_03\n");
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD16(ptr, 0x1234, size_1d/sizeof(int16_t)));
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));

	for(size_t i = 0; i < size_1d/sizeof(int16_t); i++) {
		if(host[i] != 0x1234) {
			printf("TEST CASE ID: FT_VEDA_MEM_03 failed\n");
			exit(0);
		}
	}
	printf("\nTEST CASE ID: FT_VEDA_MEM_04\n");
	memset(host, 0x00, size_1d);
	CHECK(vedaCtxStreamCnt(&stream_cnt));
	if(stream_cnt == 0)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_04 failed\n");
		exit(0);
	}
	if(stream_cnt%2 != 0)
		stream_cnt = stream_cnt - 1;
	if(stream_cnt == 6 )
		stream_cnt = 4;
	each_stream_res = (size_1d/(sizeof(int16_t)*stream_cnt)); 
	for(int i=0; i<stream_cnt; i++)
	{
		CHECK(vedaMemsetD16Async((ptr + (each_stream_res*sizeof(int16_t) * i)), 0x5678, each_stream_res,i));
	}
	CHECK(vedaCtxSynchronize());
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));

	for(size_t i = 0; i < size_1d/sizeof(int16_t); i++) {
		if(host[i] != 0x5678) {
			printf("TEST CASE ID: FT_VEDA_MEM_04 failed\n");
			exit(0);
		}
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_05\n");
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD8(ptr, 0x12, size_1d/sizeof(int8_t)));
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h8 = (int8_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_05 failed\n");
			exit(0);
		}
	}
	printf("\nTEST CASE ID: FT_VEDA_MEM_06\n");
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD32(ptr, 0x34567890, size_1d/sizeof(int32_t)));
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h32 = (int32_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int32_t); i++) {
		if(h32[i] != 0x34567890) {
			printf("TEST CASE ID: FT_VEDA_MEM_06 failed\n");
			exit(0);
		}
	}
	printf("\nTEST CASE ID: FT_VEDA_MEM_07\n");
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD64(ptr, 0x3456789012345678, size_1d/sizeof(int64_t)));
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h64 = (int64_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int64_t); i++) {
		if(h64[i] != 0x3456789012345678) {
			printf("TEST CASE ID: FT_VEDA_MEM_07 failed\n");
			exit(0);
		}
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_08\n");


	host_2d = (int8_t*)malloc(sizeof(int8_t) *w*h);
	memset(h8, 0x00, sizeof(int8_t) *w*h);
	CHECK(vedaMemsetD2D8(ptr_2d, pitch_size, 0x12, w,h));
	CHECK(vedaMemcpyDtoH(h8,ptr_2d,sizeof(int8_t) *w*h));
	for(size_t i = 0; i < ((h * pitch_size) / sizeof(int8_t)); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_08 failed\n");
			exit(0);
		}
	}
	printf("\nTEST CASE ID: FT_VEDA_MEM_09\n");
	memset(host_2d, 0x00, sizeof(int8_t) *w*h);
	CHECK(vedaMemsetD2D16(ptr_2d, pitch_size, 0x1234, 2,h));
	CHECK(vedaMemcpyDtoH(host_2d,ptr_2d,sizeof(int8_t) *w*h));
	h16 = (int16_t*)host_2d; 
	for(size_t i = 0; i < ((h * pitch_size) / sizeof(int16_t)); i++) {
		if(h16[i] != 0x1234) {
			printf("TEST CASE ID: FT_VEDA_MEM_09 failed\n");
			exit(0);
		}
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_10\n");
	memset(host_2d, 0x00, sizeof(int8_t) *w*h);
	CHECK(vedaMemsetD2D32(ptr_2d, pitch_size, 0x12345678, 1,h));
	CHECK(vedaMemcpyDtoH(host_2d,ptr_2d,sizeof(int8_t) *w*h));
	h32 = (int32_t*)host_2d; 
	for(size_t i = 0; i < ((h * pitch_size) / sizeof(int32_t)); i++) {
		if(h32[i] != 0x12345678) {
			printf("TEST CASE ID: FT_VEDA_MEM_08 failed\n");
			exit(0);
		}
	}


	printf("\nTEST CASE ID: FT_VEDA_MEM_11\n");
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD64(ptr, 0x3456789012345678, size_1d/sizeof(int64_t)));
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h64 = (int64_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int64_t); i++) {
		if(h64[i] != 0x3456789012345678) {
			printf("TEST CASE ID: FT_VEDA_MEM_11 failed\n");
			exit(0);
		}
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_12\n");
	CHECK(vedaMemsetD8(ptr, 0x00, size_1d/sizeof(int8_t)));
	memset(host, 0x12, size_1d);
	CHECK(vedaMemcpyHtoD(ptr,host,size_1d));
	memset(host, 0x00, size_1d);
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h8 = (int8_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_12 failed\n");
			exit(0);
		}
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_13\n");
	VEDAptr<char> ptr1 = 0;
	CHECK(vedaMemAlloc(&ptr1, size_1d));
	if(ptr1.size() != 64)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_13 failed\n");
		exit(0);
	}
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD8(ptr, 0x00, size_1d/sizeof(int8_t)));
	CHECK(vedaMemsetD8(ptr1, 0x12, size_1d/sizeof(int8_t)));
	CHECK(vedaMemcpyDtoD(ptr,ptr1,size_1d));
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h8 = (int8_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_13 failed\n");
			exit(0);
		}
	}
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD8(ptr, 0x00, size_1d/sizeof(int8_t)));
	CHECK(vedaMemsetD8(ptr1, 0x12, size_1d/sizeof(int8_t)));
	CHECK(vedaMemcpy(ptr,ptr1,size_1d));
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h8 = (int8_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_13 failed\n");
			exit(0);
		}
	}
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD8(ptr, 0x00, size_1d/sizeof(int8_t)));
	CHECK(vedaMemsetD8(ptr1, 0x12, size_1d/sizeof(int8_t)));
	CHECK(vedaMemcpyAsync(ptr,ptr1,size_1d,0));
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h8 = (int8_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_13 failed\n");
			exit(0);
		}
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_27\n");
	VEDAdeviceptr ptr_adv = ptr;
	ptr_adv++;
	VEDAdeviceptr ptr_base;
	size_t base_size;
	CHECK(vedaMemGetAddressRange(&ptr_base, &base_size, ptr_adv));
	printf("%p\t%p\t%p",ptr,ptr_adv,ptr_base);
	if( (VEDAdeviceptr)ptr != ptr_base || ptr.size() != base_size)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_27 failed\n");
		exit(0);
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_29\n");
	if(VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED != vedaMemFree(ptr_adv))
	{
		printf("TEST CASE ID: FT_VEDA_MEM_29 failed\n");
		exit(0);
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_14\n");
	CHECK(vedaMemsetD8(ptr, 0x12, size_1d/sizeof(int8_t)));
	CHECK(vedaCtxCreate(&c2, VEDA_CONTEXT_MODE_SCALAR, 1));
	VEDAptr<char> ptr2 = 0;
	CHECK(vedaMemAlloc(&ptr2, size_1d));
	if(ptr2.size() != 64)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_14 failed\n");
		exit(0);
	}
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD8(ptr2, 0x00, size_1d/sizeof(int8_t)));
	CHECK(vedaMemcpyDtoD(ptr2,ptr,size_1d));
	CHECK(vedaMemcpyDtoH(host,ptr2,size_1d));
	h8 = (int8_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_14 failed\n");
			exit(0);
		}
	}
	CHECK(vedaMemFree(ptr2));
	printf("\nTEST CASE ID: FT_VEDA_MEM_15\n");
	CHECK(vedaCtxSetCurrent(c1));
	memset(host, 0x00, size_1d);
	CHECK(vedaMemsetD8(ptr, 0x12, size_1d/sizeof(int8_t)));
	CHECK(vedaCtxStreamCnt(&stream_cnt));
	if(stream_cnt == 0)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_15 failed\n");
		exit(0);
	}
	if(stream_cnt%2 != 0)
		stream_cnt = stream_cnt - 1;
	if(stream_cnt == 6 )
		stream_cnt = 4;
	each_stream_res = (size_1d/(sizeof(int8_t)*stream_cnt));
	h8 = (int8_t *)host;
	for(int i=0; i<stream_cnt; i++)
	{
		CHECK(vedaMemcpyDtoHAsync(h8+(each_stream_res*sizeof(int8_t) * i),ptr+(each_stream_res*sizeof(int8_t) * i),each_stream_res,i));
	}
	CHECK(vedaCtxSynchronize());
	for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_15 %p failed\n", h8[i]);
			exit(0);
		}
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_16\n");
	memset(host, 0x12, size_1d);
	CHECK(vedaMemsetD8(ptr, 0x00, size_1d/sizeof(int8_t)));
	h8 = (int8_t *)host;
	for(int i=0; i<stream_cnt; i++)
	{
		CHECK(vedaMemcpyHtoDAsync(ptr+(each_stream_res*sizeof(int8_t) * i),h8+(each_stream_res*sizeof(int8_t) * i),each_stream_res,i));
	}
	CHECK(vedaCtxSynchronize());
	memset(host, 0x00, size_1d);
	CHECK(vedaMemcpyDtoH(host,ptr,size_1d));
	h8 = (int8_t *)host;
	for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
		if(h8[i] != 0x12) {
			printf("TEST CASE ID: FT_VEDA_MEM_16 %p failed\n", h8[i]);
			exit(0);
		}
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_17\n");
	CHECK(vedaMemGetDevice(&device_id, ptr));
	if(device_id != 0)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_17 failed\n");
		exit(0);
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_18\n");
	VEDAptr<char> ptr_free = 0;
	CHECK(vedaMemAlloc(&ptr_free, size_1d));
	CHECK(vedaMemFree(ptr_free));
	size_t ptr_free_size=-1;
	if(vedaMemSize(&ptr_free_size,ptr_free) != VEDA_ERROR_UNKNOWN_VPTR
			|| vedaMemFree(ptr_free) != VEDA_ERROR_UNKNOWN_VPTR)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_18 failed\n");
		exit(0);
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_19\n");
	int *host_mem = NULL;
	CHECK(vedaMemAllocHost((void**)&host_mem, 64));
	if(host_mem == NULL)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_19 failed\n");
		exit(0);
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_20\n");
	CHECK(vedaMemFreeHost(host_mem));

	if(VEDA_ERROR_INVALID_VALUE != vedaMemFreeHost(0))
	{
		printf("TEST CASE ID: FT_VEDA_MEM_20 failed\n");
		exit(0);
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_21\n");
	CHECK(vedaMemPtr((void**)&raw_addr, ptr));
	if(raw_addr == NULL)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_21 failed\n");
		exit(0);
	}

	printf("\nTEST CASE ID: FT_VEDA_MEM_22\n");
	size_t ptr_size = 0;
	CHECK(vedaMemSize(&ptr_size,ptr));
	if(ptr_size != 64)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_22 %d failed\n", ptr_size);
		exit(0);	
	}
	//Allocating the VEDA device memory with 0 size
	VEDAmodule mod;
        const char* modName = "libveda_test.vso";
        CHECK(vedaModuleLoad(&mod, modName));

       	VEDAfunction func;
        const char* funcName = "ve_test";
        CHECK(vedaModuleGetFunction(&func, mod, funcName));
	VEDAdeviceptr ptr22,ptr3;
        CHECK(vedaMemAlloc(&ptr22, 0));
        CHECK(vedaLaunchKernel(func, 0, ptr3, ptr22, 8));
	ptr_size = 0;
	vedaMemSize(&ptr_size,ptr22);
	if(ptr_size != 32)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_22 %d failed\n", ptr_size);
		exit(0);	
	}
	CHECK(vedaCtxSynchronize());
	CHECK(vedaMemFree(ptr22));

	printf("\nTEST CASE ID: FT_VEDA_MEM_23\n");
	VEDAdeviceptr hmemPtr = NULL;
	CHECK(vedaMemHMEM((void**)&hmemPtr, ptr));
	if(hmemPtr == NULL)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_23 failed\n");
		exit(0);	
	}
	
	printf("\nTEST CASE ID: FT_VEDA_MEM_24\n");
	ptr_size = 0;
	CHECK(vedaMemHMEMSize((void**)&hmemPtr, &ptr_size, ptr));
	if(ptr_size != 64)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_24 %d failed\n", ptr_size);
		exit(0);	
	}
	printf("\nTEST CASE ID: FT_VEDA_MEM_25\n");
	CHECK(vedaMemReport());

	printf("\nTEST CASE ID: FT_VEDA_MEM_26\n");
	size_t free=0,total=0;
	CHECK(vedaMemGetInfo(&free,&total));
	if((total-free) != 144)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_26 %d failed\n", (total-free));
		exit(0);	
	}
	
	printf("\nTEST CASE ID: FT_VEDA_MEM_28\n");
	setenv("VEDA_MEM_TRACE","1", 1);

	printf("\nTEST CASE ID: FT_VEDA_MEM_30\n");
	memset(host, 0x00, size_1d);
	if(vedaMemcpyDtoH(host,ptr,100) != VEDA_ERROR_OUT_OF_BOUNDS)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_30 failed\n");
		exit(0);
	}


	printf("\nTEST CASE ID: FT_VEDA_MEM_31\n");
	memset(host, 0x00, size_1d);
	if(vedaMemcpyHtoD(ptr,host,100) != VEDA_ERROR_OUT_OF_BOUNDS)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_31 failed\n");
		exit(0);
	}
	//Negative value function call for code coverage
	printf("\nTEST CASE ID: FT_VEDA_MEM_32:");
	VEDAresult ret = vedaMemcpyDtoHAsync(0,ptr,0,0);
        if(ret == VEDA_ERROR_INVALID_VALUE)
                printf("PASSED\n");
        else
                {printf("FAILED\n");exit(1);}
	printf("\nTEST CASE ID: FT_VEDA_MEM_33:");
	ret = vedaMemcpyHtoDAsync(ptr,0,0,0);
        if(ret == VEDA_ERROR_INVALID_VALUE)
                printf("PASSED\n");
        else
                {printf("FAILED\n");exit(1);}
	printf("\nTEST CASE ID: FT_VEDA_MEM_34:");
	ret = vedaMemcpyDtoDAsync(ptr,0,0,0);
        if(ret == VEDA_ERROR_INVALID_VALUE)
                printf("PASSED\n");
        else
                {printf("FAILED\n");exit(1);}
	printf("\nTEST CASE ID: FT_VEDA_MEM_35:");
	ret = vedaMemsetD16(0,0,0);
        if(ret == VEDA_ERROR_INVALID_VALUE)
                printf("PASSED\n");
        else
                {printf("FAILED\n");exit(1);}
	CHECK(vedaExit());
	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	printf("vedaExit()\n");
	return 0;
}
