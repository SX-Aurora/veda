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
	if(argc != 2)
		return 1;
	CHECK(vedaInit(0));
	VEDAcontext ctx;
	CHECK(vedaCtxCreate(&ctx, 0, std::atoi(argv[1])));
	CHECK(vedaExit());
	return 0;
}