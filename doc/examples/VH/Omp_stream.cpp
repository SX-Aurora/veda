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
		int cnt;
		CHECK(vedaCtxCreate(&ctx, VEDA_CONTEXT_MODE_OMP, dev));
                CHECK(vedaCtxStreamCnt(&cnt));
                if(cnt == 1)
                {
                        printf("Passed\n");
                }
                printf("Stream count in omp mode is %d for device %d\n",cnt,dev);
        }

        CHECK(vedaExit());

	return 0;
}
