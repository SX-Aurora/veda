#include <stdio.h>
int omp_hello(void)
{
  int tid, nthreads = 0;
#pragma omp parallel private(nthreads, tid)
  {
    tid = omp_get_thread_num();
    printf("Hello, World! from thread = %d\n", tid);
    if (tid == 0)
    {
      nthreads = omp_get_num_threads();
      printf("Number of threads = %d\n", nthreads);
    }
  }  /* All threads join master thread and disband */
  fflush(stdout);
  return 0;
}
