CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

PROJECT(VEDA VERSION 0.9.5.2 LANGUAGES C CXX)
SET(CMAKE_BUILD_TYPE RELEASE)

## CMake Stuff -----------------------------------------------------------------
IF(NOT ${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
	MESSAGE(FATAL "Not supported operating system. Only Linux supported!")
ENDIF()

SET(VEDA_LATEST_PREFIX "/usr/local/ve/veda")
SET(VEDA_INSTALL_PREFIX "${VEDA_LATEST_PREFIX}-${PROJECT_VERSION}")

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	SET(CMAKE_INSTALL_PREFIX ${VEDA_INSTALL_PREFIX} CACHE PATH "CMAKE install path" FORCE)
ENDIF()

INSTALL(CODE "execute_process( \
	COMMAND ${CMAKE_COMMAND} -E create_symlink \
	${CMAKE_INSTALL_PREFIX} \
	${VEDA_LATEST_PREFIX}   \
	)"
)

SET(CMAKE_INSTALL_RPATH "\$ORIGIN")
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

OPTION(BUILD_SHARED "Builds veda as shared library." ON)
IF(${BUILD_SHARED})
	SET(BUILD_TYPE SHARED)
ELSE()
	SET(BUILD_TYPE STATIC)
ENDIF()

## Download/Build AVEO ---------------------------------------------------------
SET(AVEO_REPO "https://github.com/SX-Aurora/aveo")
SET(AVEO_PATH ${CMAKE_CURRENT_BINARY_DIR}/aveo)
SET(AVEO_GIT_TAG "v0.9.15")
SET(AVEO_NFORT "/opt/nec/ve/bin/nfort" CACHE STRING "NFORT Version used to compile AVEO")

CONFIGURE_FILE("${CMAKE_CURRENT_LIST_DIR}/aveo.patch.in" "${CMAKE_CURRENT_BINARY_DIR}/aveo.patch" @ONLY)

OPTION(DOWNLOAD_AVEO "Download and build AVEO" ON)

IF(DOWNLOAD_AVEO)
	INCLUDE(ExternalProject)
	ExternalProject_Add(aveo
		BINARY_DIR		${AVEO_PATH}/src
		BUILD_COMMAND		make
		CONFIGURE_COMMAND	""
		GIT_REPOSITORY		${AVEO_REPO}
		GIT_SHALLOW		TRUE
		GIT_TAG			${AVEO_GIT_TAG}
		INSTALL_COMMAND		make install
		INSTALL_DIR		""
		PATCH_COMMAND		git apply ${CMAKE_CURRENT_BINARY_DIR}/aveo.patch
		PREFIX			${AVEO_PATH}
		SOURCE_DIR		${AVEO_PATH}/src
		STAMP_DIR		${AVEO_PATH}/stamp
		TMP_DIR			${AVEO_PATH}/tmp
	)
ENDIF()

# CMake Variables
SET(AVEO_INCLUDE	"${AVEO_PATH}/src/src/") # don't use the install path, as AVEO gets always rebuild
SET(AVEO_LIBRARY	"${AVEO_PATH}/src/install/lib64/libveo.a")
SET(AVEO_URPC_LIBRARY	"${AVEO_PATH}/src/install/lib64/liburpcVH.a")
SET(AVEO_AVEORUN	"${AVEO_PATH}/src/install/libexec/aveorun")

## Build VEDA ------------------------------------------------------------------
CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/veda_version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/veda_version.cpp)
CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/veda_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/veda_version.h)

FIND_LIBRARY(LIBUDEV_LIBRARY "libudev.so")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${AVEO_INCLUDE})
SET(VEDA_SRC
	${CMAKE_CURRENT_BINARY_DIR}/veda_version.cpp
	${CMAKE_CURRENT_BINARY_DIR}/veda_version.h
	${CMAKE_CURRENT_LIST_DIR}/VEDAcontext.cpp
	${CMAKE_CURRENT_LIST_DIR}/VEDAmodule.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_args.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_context.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_device.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_internal.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_kernel.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_mem.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_module.cpp
	${CMAKE_CURRENT_LIST_DIR}/veda_stream.cpp
)

ADD_LIBRARY(veda ${BUILD_TYPE} ${VEDA_SRC})
TARGET_LINK_LIBRARIES(veda ${AVEO_LIBRARY} ${AVEO_URPC_LIBRARY} ${LIBUDEV_LIBRARY} dl)
TARGET_LINK_OPTIONS(veda PRIVATE -lpthread)
TARGET_COMPILE_OPTIONS(veda PRIVATE
	-Wuninitialized
	-Wunknown-pragmas
	-Wunused-function
	-Wunused-label
	-Wunused-value
	-Wunused-variable
	-Wno-return-local-addr)
TARGET_LINK_OPTIONS(veda PRIVATE
	-Wl,--no-as-needed
	-Wl,-z,defs
	-Wl,--version-script=${CMAKE_CURRENT_LIST_DIR}/libveda.map
)
SET_TARGET_PROPERTIES(veda
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
IF(DOWNLOAD_AVEO)
	ADD_DEPENDENCIES(veda aveo)
ENDIF()

## Build VERA ------------------------------------------------------------------
SET(VERA_SRC
	${CMAKE_CURRENT_LIST_DIR}/vera.cpp
)
ADD_LIBRARY(vera ${BUILD_TYPE} ${VERA_SRC})
TARGET_LINK_LIBRARIES(vera veda)
TARGET_COMPILE_OPTIONS(vera PRIVATE
	-Wuninitialized
	-Wunknown-pragmas
	-Wunused-function
	-Wunused-label
	-Wunused-value
	-Wunused-variable
	-Wno-return-local-addr)
TARGET_LINK_OPTIONS(vera PRIVATE
	-Wl,--no-as-needed
	-Wl,-z,defs
	-Wl,--version-script=${CMAKE_CURRENT_LIST_DIR}/libvera.map
)
SET_TARGET_PROPERTIES(vera
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
ADD_SUBDIRECTORY(ve)

## Install Files ---------------------------------------------------------------
INSTALL(FILES 
	${CMAKE_CURRENT_BINARY_DIR}/veda_version.h
	${CMAKE_CURRENT_LIST_DIR}/veda.h 
	${CMAKE_CURRENT_LIST_DIR}/veda_enums.h 
	${CMAKE_CURRENT_LIST_DIR}/veda_macros.h 
	${CMAKE_CURRENT_LIST_DIR}/veda_types.h 
	${CMAKE_CURRENT_LIST_DIR}/vera.h
	${CMAKE_CURRENT_LIST_DIR}/vera_enums.h
	${CMAKE_CURRENT_LIST_DIR}/vera_types.h
DESTINATION include)
INSTALL(TARGETS veda vera LIBRARY DESTINATION lib64)
INSTALL(FILES ${AVEO_AVEORUN} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE DESTINATION libexec)

## CMake Find Script -----------------------------------------------------------
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/cmake/CMakeLists.txt)

## Tests -----------------------------------------------------------------------
OPTION(WITH_TESTS "Enable tests" OFF)
IF(WITH_TESTS)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/ve)
	ADD_SUBDIRECTORY(tests)
ENDIF()

## Build RPM -------------------------------------------------------------------
CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/rpm_post.sh.in ${CMAKE_CURRENT_BINARY_DIR}/rpm_post.sh)
SET(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
SET(CPACK_GENERATOR "RPM")
SET(CPACK_PACKAGE_NAME "veda-${PROJECT_VERSION}")
SET(CPACK_PACKAGE_RELEASE 1)
SET(CPACK_PACKAGE_CONTACT "Nicolas Weber")
SET(CPACK_PACKAGE_VENDOR "NEC Laboratories Europe")
SET(CPACK_PACKAGING_INSTALL_PREFIX  ${VEDA_INSTALL_PREFIX})
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VEDA Hybrid Programming API")
SET(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/sx-aurora/veda")
SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${CMAKE_CURRENT_BINARY_DIR}/rpm_post.sh)
INCLUDE(CPack)