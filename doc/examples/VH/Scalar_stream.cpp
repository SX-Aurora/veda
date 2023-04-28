#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

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

	int devcnt;
	CHECK(vedaDeviceGetCount(&devcnt));

	for(int dev = 0; dev < devcnt; dev++) {
		VEDAcontext ctx;
		int cnt, cores;
		CHECK(vedaCtxCreate(&ctx, VEDA_CONTEXT_MODE_SCALAR, dev));
        	CHECK(vedaDeviceGetAttribute(&cores,VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,dev));
            	CHECK(vedaCtxStreamCnt(&cnt));
		if(cnt == cores)
            	{
                    	printf("Passed\n");
            	}
		printf("For device %d: Stream count is %d and avaliable device core is %d\n",dev, cnt, cores);
	}
	CHECK(vedaExit());

	//In above code, we can change the default stream value by updating "VE_OMP_NUM_THREADS" env variable.
	setenv("VE_OMP_NUM_THREADS","1", 1);
        CHECK(vedaInit(0));
        CHECK(vedaDeviceGetCount(&devcnt));

        for(int dev = 0; dev < devcnt; dev++) {
                VEDAcontext ctx;
		int cnt;
                CHECK(vedaCtxCreate(&ctx, VEDA_CONTEXT_MODE_SCALAR, dev));
                CHECK(vedaCtxStreamCnt(&cnt));
                if(cnt == 1)
                {
                        printf("Passed\n");
                }
                printf("For device %d: Stream count is %d which should be same as VE_OMP_NUM_THREADS env variable i.e. 1\n",dev,cnt);
        }

        CHECK(vedaExit());

	return 0;
}
