#include <veda.h>
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

void run(VEDAcontext_mode omp, VEDAdevice device) {
	VEDAcontext ctx;
	CHECK(vedaCtxCreate(&ctx, omp, device));
	printf("vedaCtxCreate(%p, %i, %i)\n", ctx, omp, device);

	VEDAmodule mod;
	const char* modName = "libveda_test.vso";
	CHECK(vedaModuleLoad(&mod, modName));
	printf("vedaModuleLoad(%p, \"%s\")\n", mod, modName);

	VEDAfunction func;
	const char* funcName = "ve_hello_world";
	CHECK(vedaModuleGetFunction(&func, mod, funcName));
	printf("vedaModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);

	int num = 0;
	CHECK(vedaCtxStreamCnt(&num));
	printf("vedaCtxStreamCnt(%i)\n", num);

	std::vector<VEDAstream> streams;
	streams.resize(num);

	for(VEDAstream stream : streams)			CHECK(vedaLaunchKernel(func, stream, stream));
	for(VEDAstream stream : streams)			CHECK(vedaStreamSynchronize(stream));

	CHECK(vedaCtxDestroy(ctx));
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	for(auto omp : {VEDA_CONTEXT_MODE_OMP, VEDA_CONTEXT_MODE_SCALAR})
		run(omp, 0);

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}