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

	printf("\nTEST CASE ID: FT_VEDA_HMEM_01 \n");
    	VEDAcontext c1;
	CHECK(vedaCtxCreate(&c1, VEDA_CONTEXT_MODE_SCALAR, 0));
	VEDAhmemptr hmemptr_2d = 0;
	size_t pitch_size = 0, size_1d =128,each_stream_res=0;
	int w = 4, h = 4, stream_cnt=0,device_id=-1;
	char *raw_addr=NULL;
	

	VEDAptr<int> d_ptr = 0, d_ptr1;
	CHECK(vedaMemAlloc(&d_ptr, size_1d));
	if(d_ptr.size() != 128)
	{
		printf("TEST CASE ID: FT_VEDA_HMEM_01 failed\n");
		exit(0);
	}
        CHECK(vedaMemAlloc(&d_ptr1, size_1d));
        if(d_ptr1.size() != 128)
        {
                printf("TEST CASE ID: FT_VEDA_HMEM_01 failed\n");
                exit(0);
        }

	VEDAptr<char> ptr = 0;
        CHECK(vedaMemAlloc(&ptr, size_1d));
        if(ptr.size() != 128)
        {
                printf("TEST CASE ID: FT_VEDA_HMEM_01 failed\n");
                exit(0);
        }

	printf("\nTEST CASE ID: FT_VEDA_HMEM_02: vedaHMemAlloc\n"); 
       	VEDAhmemptr hmem_ptr = 0,hmem_ptr1 = 0;
        CHECK(vedaHMemAlloc(&hmem_ptr, size_1d));
        if(hmem_ptr == 0)
        {
                printf("TEST CASE ID: FT_VEDA_HMEM_02 vedaHMemAlloc failed\n");
                exit(0);
	}
        CHECK(vedaHMemAlloc(&hmem_ptr1, size_1d));
        if(hmem_ptr1 == 0)
        {
                printf("TEST CASE ID: FT_VEDA_HMEM_02 vedaHMemAlloc failed\n");
                exit(0);
        }

	printf("\nTEST CASE ID: FT_VEDA_HMEM_03: vedaHMemsetD16\n");
	printf("\nTEST CASE ID: FT_VEDA_HMEM_04: vedaHMemcpyXtoD\n");
	printf("\nTEST CASE ID: FT_VEDA_HMEM_05: vedaHMemcpyXtoD/vedaHMemcpyXtoDAsync\n");
	printf("\nTEST CASE ID: FT_VEDA_HMEM_06: vedaHMemcpyDtoX/vedaHMemcpyDtoXAsync\n\n");

	CHECK(vedaMemsetD16(d_ptr, 0x00, size_1d/sizeof(int16_t)));
	CHECK(vedaHMemsetD16(hmem_ptr, 0x1234, size_1d/sizeof(int16_t)));
	CHECK(vedaHMemcpyXtoD(d_ptr, hmem_ptr, size_1d));
	CHECK(vedaHMemcpyDtoX(hmem_ptr,d_ptr,size_1d));

	CHECK(vedaMemsetD16(d_ptr1, 0x00, size_1d/sizeof(int16_t)));
        CHECK(vedaHMemsetD16(hmem_ptr1, 0x5678, size_1d/sizeof(int16_t)));
        CHECK(vedaHMemcpyXtoDAsync(d_ptr1, hmem_ptr1, size_1d,0));
        CHECK(vedaHMemcpyDtoXAsync(hmem_ptr1,d_ptr1,size_1d,0));
	
	printf("\nTEST CASE ID: FT_VEDA_HMEM_07: vedaHMemsetD16Async\n");

	CHECK(vedaMemsetD16(d_ptr, 0x00, size_1d/sizeof(int16_t)));
	CHECK(vedaCtxStreamCnt(&stream_cnt));
	if(stream_cnt == 0)
	{
		printf("TEST CASE ID: FT_VEDA_HMEM_07:vedaHMemsetD16Async failed\n");
		exit(0);
	}
	if(stream_cnt%2 != 0)
		stream_cnt = stream_cnt - 1;
	if(stream_cnt == 6 )
		stream_cnt = 4;
	each_stream_res = (size_1d/(sizeof(int16_t)*stream_cnt)); 
	for(int i=0; i<stream_cnt; i++)
	{
		CHECK(vedaHMemsetD16Async((hmem_ptr + (each_stream_res*sizeof(int16_t) * i)), 0x5678, each_stream_res,i));
	}
        printf("\nTEST CASE ID: FT_VEDA_HMEM_08: vedaHMemcpyXtoD\n");
	CHECK(vedaCtxSynchronize());
	CHECK(vedaHMemcpyXtoD(d_ptr,hmem_ptr,size_1d));
	
	printf("\nTEST CASE ID: FT_VEDA_HMEM_09: vedaHMemsetD8\n");
	printf("\nTEST CASE ID: FT_VEDA_HMEM_10: vedaHMemsetD8Async\n");
	CHECK(vedaMemsetD8(d_ptr, 0x00, size_1d/sizeof(int8_t)));
	CHECK(vedaHMemsetD8(hmem_ptr, 0x12, size_1d/sizeof(int8_t)));
	CHECK(vedaHMemcpyXtoD(d_ptr, hmem_ptr, size_1d));

        CHECK(vedaMemsetD8(d_ptr, 0x00, size_1d/sizeof(int8_t)));
        each_stream_res = (size_1d/(sizeof(int8_t)*stream_cnt));
        for(int i=0; i<stream_cnt; i++)
        {
                CHECK(vedaHMemsetD8Async((hmem_ptr + (each_stream_res*sizeof(int8_t) * i)), 0x56, each_stream_res,i));
        }
        CHECK(vedaCtxSynchronize());
	
	printf("\nTEST CASE ID: FT_VEDA_HMEM_11: vedaHMemsetD32\n");
	printf("\nTEST CASE ID: FT_VEDA_HMEM_12: vedaHMemsetD32Async\n");
	CHECK(vedaMemsetD32(d_ptr, 0x00, size_1d/sizeof(int32_t)));
	CHECK(vedaHMemsetD32(hmem_ptr, 0x34567890, size_1d/sizeof(int32_t)));
	CHECK(vedaHMemcpyXtoD(d_ptr, hmem_ptr ,size_1d));

        CHECK(vedaMemsetD32(d_ptr, 0x00, size_1d/sizeof(int32_t)));
        each_stream_res = (size_1d/(sizeof(int32_t)*stream_cnt));
        for(int i=0; i<stream_cnt; i++)
        {
                CHECK(vedaHMemsetD32Async((hmem_ptr + (each_stream_res*sizeof(int32_t) * i)), 0x56, each_stream_res,i));
        }
        CHECK(vedaCtxSynchronize());
        CHECK(vedaHMemcpyXtoD(d_ptr,hmem_ptr,size_1d));

	printf("\nTEST CASE ID: FT_VEDA_HMEM_13: vedaHMemsetD64\n");
	printf("\nTEST CASE ID: FT_VEDA_HMEM_14: vedaHMemsetD64Async\n");
	CHECK(vedaMemsetD32(d_ptr, 0x00, size_1d/sizeof(int64_t)));
	CHECK(vedaHMemsetD64(hmem_ptr, 0x3456789012345678, size_1d/sizeof(int64_t)));
	CHECK(vedaHMemcpyXtoD(d_ptr, hmem_ptr ,size_1d));

        CHECK(vedaMemsetD64(d_ptr, 0x00, size_1d/sizeof(int64_t)));
        each_stream_res = (size_1d/(sizeof(int64_t)*stream_cnt));
        for(int i=0; i<stream_cnt; i++)
        {
                CHECK(vedaHMemsetD64Async((hmem_ptr + (each_stream_res*sizeof(int64_t) * i)), 0x56, each_stream_res,i));
        }
        CHECK(vedaCtxSynchronize());
        CHECK(vedaHMemcpyXtoD(d_ptr,hmem_ptr,size_1d));
        
	printf("\nTEST CASE ID: FT_VEDA_HMEM_15: vedaHMemPtr\n");
        CHECK(vedaHMemPtr((void**)&raw_addr, hmem_ptr));
        if(raw_addr == NULL)
        {
              printf("TEST CASE ID: FT_VEDA_HMEM_15: vedaHMemPtr\n failed\n");
              exit(0);
        }

	printf("\nTEST CASE ID: FT_VEDA_HMEM_16: vedaHMemcpy\n"); 
	void *updated_data, *dst_data;
        updated_data = (int16_t *)malloc(sizeof(int16_t));
	dst_data = (int16_t *)malloc(sizeof(int16_t));
	int16_t *a = (int16_t *)updated_data;
        a[0] = 0x1234;
	
        CHECK(vedaHMemcpy(dst_data,updated_data,sizeof(int16_t)));
	int16_t *b =  (int16_t *)dst_data;
        if(b[0] != 0x1234) {
                        printf("TEST CASE ID: FT_VEDA_HMEM_16:vedaHMemcpy failed %x\n",b[0]);
                      exit(0);
        }
	
        printf("\nTEST CASE ID: FT_VEDA_HMEM_17: vedaHMemsetD128\n");
	printf("\nTEST CASE ID: FT_VEDA_HMEM_18: vedaHMemsetD128Async\n");
        CHECK(vedaMemsetD32(d_ptr, 0x00, size_1d/sizeof(int64_t)));
        CHECK(vedaHMemsetD128(hmem_ptr, 0x12,0x12,size_1d/(2*sizeof(int64_t))));
        CHECK(vedaHMemcpyXtoD(d_ptr, hmem_ptr ,size_1d));

        CHECK(vedaMemsetD32(d_ptr, 0x00, size_1d/sizeof(int64_t)));
        CHECK(vedaHMemsetD128Async(hmem_ptr , 0x12,0x12, size_1d/(2*sizeof(int64_t)),0));
        CHECK(vedaHMemcpyXtoD(d_ptr, hmem_ptr ,size_1d));

	printf("\nTEST CASE ID: FT_VEDA_HMEM_19: vedaHMemFree\n");
        CHECK(vedaHMemFree(hmem_ptr));
        CHECK(vedaMemFree(d_ptr));

	CHECK(vedaExit());
	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	printf("vedaExit()\n");
	return 0;
}
