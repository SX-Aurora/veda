#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <unistd.h>

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
   int devcnt = 0;
   CHECK(vedaDeviceGetCount(&devcnt));
   for(int dev = 0; dev < devcnt; dev++) {
	VEDAcontext ctx;
	CHECK(vedaCtxCreate(&ctx, VEDA_CONTEXT_MODE_SCALAR, dev));

	VEDAmodule mod;
	const char* modName = "./libvehello.vso";
	CHECK(vedaModuleLoad(&mod, modName));

	VEDAfunction func;
	const char* funcName = "hello_world";
	CHECK(vedaModuleGetFunction(&func, mod, funcName));

	int num = 0;
	CHECK(vedaCtxStreamCnt(&num));

	for(int stream =0; stream<num; stream++){			
		printf("stream =%d \n",stream);
		CHECK(vedaLaunchKernel(func, stream, 0));
	}
	for(int stream = 0; stream < num; stream++)	
		CHECK(vedaStreamSynchronize(stream));

	CHECK(vedaCtxDestroy(ctx));
   }
   CHECK(vedaExit());
   return 0;
}
