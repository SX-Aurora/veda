FIND_PATH	(VEDA_DIR "include/veda.h"	PATHS "/usr/local/ve/veda/" "${CMAKE_CURRENT_LIST_DIR}/../")
FIND_LIBRARY(VEDA_LIBRARY "libveda.so" "libveda.a" PATHS "${VEDA_DIR}/lib")
FIND_LIBRARY(VEDA_DEVICE_LIBRARY "libveda.vso" PATHS "${VEDA_DIR}/lib")
FIND_PATH	(VEDA_INCLUDES "veda.h" PATHS "${VEDA_DIR}/include")
