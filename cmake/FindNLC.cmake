INCLUDE(${CMAKE_CURRENT_LIST_DIR}/nec/VE_Helper.cmake)

IF(NOT NLC_FOUND)
	SET(NLC_PATH)
	SET(NLC_FOUND FALSE)

	_NEC_FIND_FOLDER("/opt/nec/ve/nlc/*" NLC_PATH NLC_VERSION)
	IF(NOT MPI_VERSION STREQUAL "")
		MESSAGE(STATUS "Found NLC ${NLC_VERSION}")
		SET(NLC_FOUND TRUE)
	ELSEIF(${NLC_FIND_REQUIRED})
		MESSAGE(FATAL_ERROR "Unable to detect NEC NLC")
	ENDIF()

	SET(NLC_FOUND ${NLC_FOUND} CACHE BOOL "")
	SET(NLC_PATH ${NLC_PATH} CACHE PATH "")
	SET(NLC_VERSION ${NLC_VERSION} CACHE STRING "")

	MARK_AS_ADVANCED(NLC_FOUND NLC_PATH NLC_VERSION)
ENDIF()