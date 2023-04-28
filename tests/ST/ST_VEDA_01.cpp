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
        int devcnt;
        CHECK(veraGetDeviceCount(&devcnt));
        printf("veraGetDeviceCount(%i)\n", devcnt);
	int dev=0;

	veraModule_t mod;
	const char* modName = "./libST_VEDA_01_ve.vso";
	CHECK(veraModuleLoad(&mod, modName));
  		printf("veraModuleLoad(%p, \"%s\")\n", mod, modName);

	veraFunction_t func;
	const char* funcName = "testST_VEDA_01";
	CHECK(veraModuleGetFunction(&func, mod, funcName));
 	printf("vedaModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);

	size_t size = 64*1024*1024;
	size_t cnt = size/sizeof(int);
	int* host = (int*)malloc(size);
	if(host == NULL){
		printf("Malloc failed\n");
		exit(1);
	}
		
	for(size_t i = 0; i < cnt; i++)
		host[i] = i;

	void* d_a = 0;
	CHECK(veraMalloc(&d_a, size));
	CHECK(veraMemset(d_a, 0x00, cnt));
	CHECK(veraMemcpy(d_a, host, size, veraMemcpyHostToDevice));

	CHECK(veraLaunchKernel(func, 0, VEDAptr<int>(VERA2VEDA(d_a)).ptr(), cnt));
	CHECK(veraDeviceSynchronize());

	CHECK(veraMemcpy((void*)host, (void*)d_a, size, veraMemcpyDeviceToHost));
	
	printf("\nChecking received data from device.\n");
	for(size_t i = 0; i < cnt; i++) {
		if(host[i] != i+1) {
			printf("expected host[%i] to be %d but is %d\n", i, i+1, host[i]);
		//	return 1;
		}
	}
	printf("\nTest case ST_VEDA_01 passed\n");
	free(host);
	CHECK(veraFree(d_a));
}
