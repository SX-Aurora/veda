#include <iostream>
#include <veda.h>

#define VEDA(err) check(err, __FILE__, __LINE__)

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char *name, *str;
		vedaGetErrorName	(err, &name);
		vedaGetErrorString	(err, &str);
		std::cout<<name<<":"<<str<<" "<<" @ "<<file<<":"<<line<<std::endl;
		_Exit(1);
	}
}

int main(int argc, char** argv) {
	std::cout<<"Hello World from Host!"<<std::endl;

	VEDA(vedaInit(0));

	VEDAcontext ctx;
	VEDA(vedaDevicePrimaryCtxRetain(&ctx, 0));
	VEDA(vedaCtxPushCurrent(ctx));

	VEDAmodule mod;
	VEDA(vedaModuleLoad(&mod, "./libhello_world_device.vso"));

	VEDAfunction func;
	VEDA(vedaModuleGetFunction(&func, mod, "hello_world"));

	VEDAargs args;
	VEDA(vedaArgsCreate(&args));
	VEDA(vedaLaunchKernelEx(func, 0, args, 1, NULL));
	
	VEDA(vedaCtxSynchronize());
	VEDA(vedaExit());

	return 0;
}
