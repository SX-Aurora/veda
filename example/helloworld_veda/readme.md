# Hello World

## How to build:
```bash
mkdir build
cd build
cmake ..
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
