CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

PROJECT(VEDA_DELAYED_MALLOC LANGUAGES C)

SET(CMAKE_MODULE_PATH /usr/local/ve/veda/cmake /opt/nec/ve/share/veda/cmake)
FIND_PACKAGE(VEDA)

ENABLE_LANGUAGE(VEDA_C)

SET(CMAKE_BUILD_TYPE RELEASE)
INCLUDE_DIRECTORIES(${VEDA_INCLUDE_DIRS})

ADD_EXECUTABLE		(delayed_malloc host.c)
TARGET_LINK_LIBRARIES	(delayed_malloc ${VEDA_LIBRARIES})
ADD_LIBRARY		(delayed_malloc_device SHARED device.vc)
TARGET_LINK_LIBRARIES	(delayed_malloc_device ${VEDA_DEVICE_LIBRARY})