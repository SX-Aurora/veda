INTEGER FUNCTION OMP_HELLO()
  INTEGER :: TID = 0
  INTEGER :: NTHREADS = 0
!$OMP PARALLEL PRIVATE(TID, NTHREADS)
  TID = omp_get_thread_num()
  WRITE(*,*) "Hello, World! from thread = ", TID
  IF ( TID == 0 ) THEN
    NTHREADS = omp_get_num_threads()
    OMP_HELLO = NTHREADS
    WRITE(*,*) "Number of threads = ", NTHREADS
  END IF
!$OMP END PARALLEL
END FUNCTION OMP_HELLO
