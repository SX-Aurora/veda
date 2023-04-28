CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

PROJECT(VEDA VERSION 2.0.1 LANGUAGES C CXX)

IF(NOT Illyrian_FOUND AND VEDA_DIST_TYPE AND NOT (VEDA_DIST_TYPE STREQUAL "PYTHON"))
	SET(DPATH ${CMAKE_BINARY_DIR}/dependencies)
	FILE(MAKE_DIRECTORY ${DPATH})

	# Build Illyrian -----------------------------------------------
	EXECUTE_PROCESS(
		COMMAND git clone https://github.com/nec-research/illyrian
		WORKING_DIRECTORY ${DPATH})
	FILE(MAKE_DIRECTORY ${DPATH}/illyrian/build)
	EXECUTE_PROCESS(
		COMMAND ${CMAKE_COMMAND} -DILLYRIAN_NO_PYTHON=ON ${DPATH}/illyrian
		WORKING_DIRECTORY ${DPATH}/illyrian/build)
	EXECUTE_PROCESS(
		COMMAND ${CMAKE_COMMAND} --build ${DPATH}/illyrian/build --target install)
	SET(ILLYRIAN_CMAKE ${DPATH}/illyrian/build/dist/illyrian/cmake)

	# Build Tungl --------------------------------------------------
	EXECUTE_PROCESS(
		COMMAND git clone https://github.com/nec-research/tungl
		WORKING_DIRECTORY ${DPATH})
	FILE(MAKE_DIRECTORY ${DPATH}/tungl/build)
	EXECUTE_PROCESS(
		COMMAND ${CMAKE_COMMAND} -DILLYRIAN_NO_PYTHON=ON -DCMAKE_MODULE_PATH=${ILLYRIAN_CMAKE} -DCMAKE_INSTALL_PREFIX=${DPATH}/tungl/build/dist ${DPATH}/tungl
		WORKING_DIRECTORY ${DPATH}/tungl/build)
	EXECUTE_PROCESS(
		COMMAND ${CMAKE_COMMAND} --build ${DPATH}/tungl/build --target install)
	SET(TUNGL_CMAKE ${DPATH}/tungl/build/dist/tungl/cmake)

	SET(CMAKE_MODULE_PATH ${ILLYRIAN_CMAKE} ${TUNGL_CMAKE} CACHE STRING "")

	SET(ILLYRIAN_NO_PYTHON ON CACHE BOOL "")
ENDIF()

FIND_PACKAGE(Illyrian 0.2.1 REQUIRED)
ILLYRIAN_PROJECT(
	C_STANDARD	17
	CXX_STANDARD	17
	INSTALL_PREFIX	"/usr/local/ve/"
	SUPPORTED_OS	"Linux"
)

ILLYRIAN_FIND_PACKAGE(Tungl VERSION 0.1.1 REQUIRED PYTHON "tungl/cmake" PATHS "/usr/local/nle/tungl/cmake")

## CMake Stuff -----------------------------------------------------------------
ILLYRIAN_OPTIONS(VEDA_BUILD_TYPE SHARED STATIC)
ILLYRIAN_OPTIONS(VEDA_DIST_TYPE LOCAL VEOS PYTHON)

## Set Build Dependent Properties ----------------------------------------------
INCLUDE(dist/CMakeLists.txt)

ADD_SUBDIRECTORY(cmake)
ADD_SUBDIRECTORY(src)

## Tests -----------------------------------------------------------------------
OPTION(VEDA_WITH_TESTS "Enable tests" OFF)
IF(VEDA_WITH_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF()

## Licenses --------------------------------------------------------------------
IF(VEDA_DIST_TYPE STREQUAL VEOS)
	INSTALL(FILES ${CMAKE_CURRENT_LIST_DIR}/LICENSE DESTINATION ${VEDA_INSTALL_PATH} RENAME VEDA_LICENSE)
ELSE()
	INSTALL(FILES ${CMAKE_CURRENT_LIST_DIR}/LICENSE DESTINATION ${VEDA_INSTALL_PATH})
	INSTALL(FILES ${AVEO_PATH}/src/COPYING DESTINATION ${VEDA_INSTALL_PATH} RENAME AVEO_LICENSE)
	INSTALL(FILES ${AVEO_PATH}/src/prereqs/ve-urpc/LICENSE DESTINATION ${VEDA_INSTALL_PATH} RENAME VEURPC_LICENSE)
ENDIF()
