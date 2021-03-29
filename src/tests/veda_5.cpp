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

	for(int devA = 0; devA < devcnt; devA++) {
		for(int devB = 0; devB < devcnt; devB++) {
			float distance = 0;
			CHECK(vedaDeviceDistance(&distance, devA, devB));
			printf("Distance %i >> %i = %f (%s)\n", devA, devB, distance, distance == 0 ? "same device" : distance == 0.5f ? "different numa node" : "different device");
		}
	}

	CHECK(vedaExit());
	return 0;
}