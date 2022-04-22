
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
  if(argc<=1) {
      printf("Pass the program iteration count to run for longer time\ni.e. $./test_binary 25000 for 1 hour approx\n");
      exit(1);
  }
  int iteration_count = atoi(argv[1]);	
  size_t count=0;
  while(count < iteration_count){
	int devcnt=0, cnt=0;
	int size_1d =  1024;
        CHECK(vedaInit(0));
	CHECK(vedaDeviceGetCount(&devcnt));
	VEDAcontext cont;
        for(int dev = 0; dev < devcnt; dev++) {
                CHECK(vedaCtxCreate(&cont, VEDA_CONTEXT_MODE_OMP, dev));
                CHECK(vedaCtxStreamCnt(&cnt));
		VEDAptr<char> ptr_free = 0;
		CHECK(vedaMemAlloc(&ptr_free, size_1d));
		if(ptr_free.size() != 1024)
		{
			printf("Memory allocation failed\n");
			printf("Exit %d\n",count);
			exit(0);
		}
		CHECK(vedaMemFree(ptr_free));
                CHECK(vedaCtxDestroy(cont));
        }
        CHECK(vedaExit());
	printf("devcnt=%d count=%d\n",devcnt,count);
	count++;
  }
}
