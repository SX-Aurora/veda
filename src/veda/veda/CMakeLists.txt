## Build VEDA ------------------------------------------------------------------
CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/internal/CMakeLists.txt)
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/cpp/CMakeLists.txt)

LIST(APPEND VEDA_SRC
	${CMAKE_CURRENT_LIST_DIR}/veda.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_args.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_context.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_device.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_hmem.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_hmemset.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_mem.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_memset.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_module.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_stream.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_profiler.cpp
)

## Install Files ---------------------------------------------------------------
INSTALL(FILES
	${CMAKE_CURRENT_BINARY_DIR}/version.h
	${CMAKE_CURRENT_LIST_DIR}/api.h
	${CMAKE_CURRENT_LIST_DIR}/api_cpp_memset.h
	${CMAKE_CURRENT_LIST_DIR}/api_cpp_args.h
	${CMAKE_CURRENT_LIST_DIR}/api_cpp_launch.h
	${CMAKE_CURRENT_LIST_DIR}/enums.h
	${CMAKE_CURRENT_LIST_DIR}/macros.h
	${CMAKE_CURRENT_LIST_DIR}/types.h
	${CMAKE_CURRENT_LIST_DIR}/ptr.h
DESTINATION ${VEDA_INSTALL_PATH}/include/veda)