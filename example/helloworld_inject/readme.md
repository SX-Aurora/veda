# Hello World

If you have an existing CPU application, you can use the ```InjectVE.cmake``` from VEDA to replace the host C/CXX/Fortran compiler with the NEC NCC compiler.

## How to build:
```bash
mkdir build
cd build
cmake -D /usr/local/ve/veda/cmake/InjectVE.cmake ..
make
```

## How to run:
```bash
VE_LD_LIBRARY_PATH=/usr/local/ve/veda/lib:/opt/nec/ve/veos/lib64:. ./hello_world
```

## Expected output:
```
Hello World from Host!
Hello World from Device!
```
