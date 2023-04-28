#include <stdio.h>
#include <stdlib.h>
#include <veda.h>

#define VEDA(err) check(err, __FILE__, __LINE__)

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char *name, *str;
		vedaGetErrorName	(err, &name);
		vedaGetErrorString	(err, &str);
		printf("%s: %s @ %s:%i\n", name, str, file, line);
		exit(1);
	}
}

int main(int argc, char** argv) {
	printf("Hello World from Host!\n");

	VEDA(vedaInit(0));

	VEDAcontext ctx;
	VEDA(vedaDevicePrimaryCtxRetain(&ctx, 0));
	VEDA(vedaCtxPushCurrent(ctx));

	VEDAmodule mod;
	VEDA(vedaModuleLoad(&mod, "./libhello_world_device.vso"));

	VEDAfunction func;
	VEDA(vedaModuleGetFunction(&func, mod, "hello_world_"));

	VEDAargs args;
	VEDA(vedaArgsCreate(&args));
	VEDA(vedaLaunchKernel(func, 0, args));
	VEDA(vedaArgsDestroy(args));
	
	VEDA(vedaCtxSynchronize());
	VEDA(vedaExit());

	return 0;
}
