# Restrictions

### Limitations/Known Problems:
 - VEDA only supports one VEDAcontext per device.
 - No unified memory space (yet).
 - VEDA by default uses the current workdirectory for loading modules. This behavior can be changed by using the env var VE_LD_LIBRARY_PATH.
 - Due to compiler incompatibilities it can be necessary to adjust the CMake variable ${AVEO_NFORT} to another compiler.

### VEDA library has the following restrictions:
 - VEDA library supports up to maximum 10 attached VE devices.
 - VEDA library can instantiate only single VEDA context per VEDA device.
 - Maximum 4,194,204 memory allocation can be performed at a time per VEDA device.
 - Maximum 64 MB of data transfer can be done to VEDA device function.
 - Shared objects to be loaded with VEDA must be linked with libnc++.so.
