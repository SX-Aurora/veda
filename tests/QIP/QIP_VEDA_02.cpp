#include <veda.h>
#include <vera.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <mpi.h>

#define CHECK(err) check(err, __FILE__, __LINE__)
#define MAX_LEN 256

void check(VEDAresult err, const char* file, const int line) {
        if(err != VEDA_SUCCESS) {
                const char* name = 0;
                vedaGetErrorName(err, &name);
                printf("Error: %i %s @ %s (%i)\n", err, name, file, line);
                assert(false);
                exit(1);
        }
}
int main(int argc, char **argv) {
    printf("Hello World from Host!\n");
    int devcnt;
    CHECK(veraGetDeviceCount(&devcnt));
    printf("veraGetDeviceCount(%i)\n", devcnt);

    int *host=0;
    void* d_a = 0;
    int mp_size, rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mp_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    size = mp_size*MAX_LEN;
    if (rank == 0) {
        host = (int*)malloc(size*sizeof(int));
        if(host == NULL){
                printf("Malloc failed\n");
                exit(1);
        }
        for(size_t i = 0; i < size; i++)
                host[i] = i;
    }
    int* tmp = (int*)malloc(MAX_LEN*sizeof(int));
    CHECK(veraMalloc(&d_a, MAX_LEN*sizeof(int)));
    CHECK(veraMemset(d_a, 0x00, MAX_LEN));
    veraModule_t mod;
    const char* modName = "./libST_VEDA_02_ve.vso";
    CHECK(veraModuleLoad(&mod, modName));
    printf("veraModuleLoad(%p, \"%s\")\n", mod, modName);

    veraFunction_t func;
    const char* funcName = "testST_VEDA_02";
    CHECK(veraModuleGetFunction(&func, mod, funcName));
    printf("vedaModuleGetFunction(%p, %p, \"%s\"),rank=%d\n", func, mod, funcName,rank);
    CHECK(veraLaunchKernel(func, 0, VEDAptr<int>(VERA2VEDA(d_a)).ptr(),MAX_LEN, rank*MAX_LEN));

    CHECK(veraStreamSynchronize(0));
    CHECK(veraMemcpy((void*)tmp, (void*)d_a, MAX_LEN*sizeof(int), veraMemcpyDeviceToHost));
    MPI_Gather(tmp, MAX_LEN, MPI_INT, host, MAX_LEN, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("\nChecking received data from device.\n");
        for(size_t i = 0; i < size; i++) {
                if(host[i] != i+1) {
                        printf("expected host[%i] to be %d but is %d\n", i, i+1, host[i]);
                        return 1;
                }
        }
        printf("\nTest case QIP_VEDA_02 passed\n");
        free(host);
        CHECK(veraFree(d_a));
    }

    MPI_Finalize();
    return 0;
}

