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

template<typename T>
void test_memset_2d(const T value, const size_t w, const size_t h) {
	VEDAdeviceptr ptr;
	CHECK(vedaMemAllocAsync(&ptr, w * h * sizeof(T), 0));
	printf("\nTEST CASE ID: FT_VEDA_MEM_08/%llu\n", sizeof(T) * 8);

	auto items = w * h;
	auto bytes = items * sizeof(T);

	auto host = new T[items];
	memset(host, 0, bytes);

	auto pitch_size = h * sizeof(T);
	if constexpr (sizeof(T) == 1)	CHECK(vedaMemsetD2D8 (ptr, pitch_size, value, w, h));
	if constexpr (sizeof(T) == 2)	CHECK(vedaMemsetD2D16(ptr, pitch_size, value, w, h));
	if constexpr (sizeof(T) == 4)	CHECK(vedaMemsetD2D32(ptr, pitch_size, value, w, h));
	if constexpr (sizeof(T) == 8)	CHECK(vedaMemsetD2D64(ptr, pitch_size, value, w, h));

	CHECK(vedaMemcpyDtoH(host, ptr, bytes));

	for(size_t H = 0; H < h; H++) {
		for(size_t W = 0; W < w; W++) {
			auto x = host[W + H * pitch_size / sizeof(T)];
			printf("%llu ", int64_t(x));
			if(x != value) {
				printf("TEST CASE ID: FT_VEDA_MEM_08/%llu failed, expected %llX, but found %llX at index %llux%llu\n", sizeof(T) * 8, int64_t(value), int64_t(x), W, H);
				exit(0);
			}
		}
		printf("\n");
	}
	CHECK(vedaMemFreeAsync(ptr, 0));
	delete[] host;
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	int devcnt = 0;
	CHECK(vedaDeviceGetCount(&devcnt));

	printf("\nTEST CASE ID: FT_VEDA_MEM_01\n");
    	VEDAcontext c1, c2;
	CHECK(vedaCtxCreate(&c1, VEDA_CONTEXT_MODE_SCALAR, 0));
	size_t allocated = 0;
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
	allocated += size_1d;
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
	allocated += w*h*sizeof(char);
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

	test_memset_2d((int8_t)0x12, w, h);
	test_memset_2d((int16_t)0x1234, w, h);
	test_memset_2d((int32_t)0x12345678, w, h);
	test_memset_2d((int64_t)0x0123456789ABCDEFll, w, h);

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
	allocated += size_1d;
	if(ptr1.size() != size_1d)
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

	if(devcnt > 1)
	{
		printf("\nTEST CASE ID: FT_VEDA_MEM_14\n");
		CHECK(vedaMemsetD8(ptr, 0x12, size_1d/sizeof(int8_t)));
		CHECK(vedaCtxCreate(&c2, VEDA_CONTEXT_MODE_SCALAR, 1));
		VEDAptr<char> ptr2 = 0;
		CHECK(vedaMemAlloc(&ptr2, size_1d));
		allocated += ptr2.size();
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
		printf("devcnt=%d\n",devcnt);
		for(size_t i = 0; i < size_1d/sizeof(int8_t); i++) {
			if(h8[i] != 0x12) {
				printf("TEST CASE ID: FT_VEDA_MEM_14 failed\n");
				exit(0);
			}
		}
		allocated -= ptr2.size();
		CHECK(vedaMemFree(ptr2));
	}
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
	allocated += ptr_free.size();
	allocated -= ptr_free.size();
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
	allocated += ptr_size;
	if(ptr_size != 32)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_22 %d failed\n", ptr_size);
		exit(0);	
	}
	CHECK(vedaCtxSynchronize());
	allocated -= ptr_size;
	CHECK(vedaMemFree(ptr22));

// DISABLED, vedaMemHMEMSize no longer exists because VEDA no longer tracks HMEM ptrs
#if 0 
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
#endif

	printf("\nTEST CASE ID: FT_VEDA_MEM_26\n");
	size_t free=0,total=0;
	CHECK(vedaMemGetInfo(&free,&total));
	if((total-free) != allocated)
	{
		printf("TEST CASE ID: FT_VEDA_MEM_26 failed. Reported %llu but expected %llu\n", (total-free), allocated);
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
