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
	VEDA(vedaModuleLoad(&mod, "libvefortran.vso"));

	VEDAfunction func;
	VEDA(vedaModuleGetFunction(&func, mod, "sub1_"));

	VEDAargs args;
	VEDA(vedaArgsCreate(&args));
	size_t x = 42;
	size_t y = 1;
	VEDA(vedaArgsSetStack(args, 0, &x, VEDA_ARGS_INTENT_IN, sizeof(x)));
	VEDA(vedaArgsSetStack(args, 1, &y, VEDA_ARGS_INTENT_OUT, sizeof(y)));
	VEDA(vedaLaunchKernel(func, 0, args));
	VEDA(vedaCtxSynchronize());
	printf("SUB1 return %lu\n",y);
	VEDA(vedaExit());

	return 0;
}
