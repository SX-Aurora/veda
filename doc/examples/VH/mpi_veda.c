#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <veda.h>
#define CHECK(err) check(err, __FILE__, __LINE__)
#define MAX_LEN (1024*1024)
void check(VEDAresult err, const char* file, const int line) {
        if(err != VEDA_SUCCESS) {
                const char* name = 0;
                vedaGetErrorName(err, &name);
                printf("Error: %i %s @ %s (%i)\n", err, name, file, line);
                assert(0);
                exit(1);
        }
}

int
main()
{
    int myrank;
    int nprocs;
    int nelems = MAX_LEN;
    int dev,cnt;
    uint64_t rc;
    uint64_t id;

    MPI_Init( 0, 0 );
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
    MPI_Comm_size( MPI_COMM_WORLD, &nprocs );

    printf( "myrank = %d, nprocs = %d, started\n", myrank, nprocs );
    fflush( stdout );

    CHECK(vedaInit(0));
    VEDAcontext cont;
    CHECK(vedaDevicePrimaryCtxRetain(&cont, 0));
    CHECK(vedaCtxPushCurrent(cont));
    CHECK(vedaCtxStreamCnt(&cnt));

    printf( "myrank = %d, nprocs = %d, context = %16p, created\n", myrank, nprocs, cont );
    VEDAmodule mod;
    const char* modName = "./libmpive.vso";
    CHECK(vedaModuleLoad(&mod, modName));

    //
    // Allocate VE memory
    //

    VEDAhmemptr vebuf;
    CHECK(vedaHMemAlloc(&vebuf, sizeof(int) * nelems));
    printf( "myrank = %d, nprocs = %d, vebuf = %16p, allocated\n", myrank, nprocs, vebuf );

    //
    // Call VE function 'init' to initialize VE memory
    //
    VEDAargs args1;
    CHECK(vedaArgsCreate(&args1));
    CHECK(vedaArgsSetHMEM(args1, 0, vebuf));
    CHECK(vedaArgsSetI32(args1, 1, myrank));
    CHECK(vedaArgsSetI32(args1, 2, nelems));

    VEDAfunction func1;
    const char* funcName1 = "init";
    CHECK(vedaModuleGetFunction(&func1, mod, funcName1));
    CHECK(vedaLaunchKernel(func1, 0, args1));
    rc = vedaCtxSynchronize();
    printf( "myrank = %d, nprocs = %d, rc=%d, VE offload\n", myrank, nprocs, rc );

    int peer = myrank^1;
    if ( peer >= nprocs ) peer = MPI_PROC_NULL;

    //
    // Call VE function 'check' to check initialized VE memory
    //
    VEDAargs args2;
    CHECK(vedaArgsCreate(&args2));
    CHECK(vedaArgsSetHMEM(args2, 0, vebuf));
    CHECK(vedaArgsSetI32(args2, 1, myrank));
    CHECK(vedaArgsSetI32(args2, 2, nelems));

    VEDAfunction func2;
    const char* funcName2 = "check";
    CHECK(vedaModuleGetFunction(&func2, mod, funcName1));
    CHECK(vedaLaunchKernel(func2, 0, args2));
    rc = vedaCtxSynchronize();
    printf( "myrank = %d, nprocs = %d, rc=%d, VE offload\n", myrank, nprocs, rc );

    MPI_Status        status;
    MPI_Barrier(MPI_COMM_WORLD);

    //
    // MPI calls on VH with VE memory buffers
    //

    if ( myrank%2 == 0) {
        MPI_Send((void *)vebuf, MAX_LEN, MPI_INT, peer, 0, MPI_COMM_WORLD);
    }
    else {
        MPI_Recv((void *)vebuf, MAX_LEN, MPI_INT, peer, 0, MPI_COMM_WORLD, &status );
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //
    // Call VE function 'check' to check VE memory after MPI calls
    //
    VEDAargs args3;
    CHECK(vedaArgsCreate(&args3));
    CHECK(vedaArgsSetHMEM(args3, 0, vebuf));
    CHECK(vedaArgsSetI32(args3, 1, myrank));
    CHECK(vedaArgsSetI32(args3, 2, nelems));
    CHECK(vedaLaunchKernel(func2, 0, args3)); //func2 is check()

    rc = vedaCtxSynchronize();
    printf( "myrank = %d, nprocs = %d, rc=%d, VE offload\n", myrank, nprocs, rc );

    // Verify
    uint64_t result;
    MPI_Reduce( &rc, &result, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD );
    if ( myrank == 0 ) {
        printf( "Result : %s\n", result ? "Failed" : "Success" );
        fflush( stdout );
    }

    vedaArgsDestroy(args1);
    vedaArgsDestroy(args2);
    vedaArgsDestroy(args3);

    CHECK(vedaHMemFree(vebuf));
    CHECK(vedaCtxDestroy(cont));

    MPI_Finalize();
    vedaExit();
    return 0;
}
