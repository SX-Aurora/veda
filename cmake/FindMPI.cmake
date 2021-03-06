INCLUDE(${CMAKE_CURRENT_LIST_DIR}/nec/VE_Helper.cmake)

IF(NOT MPI_FOUND)
	# Try finding mpinc++ through the path variable
	FIND_PROGRAM(NEC_MPI mpinc++)
	IF(NEC_MPI)
		STRING(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" MPI_VERSION ${NEC_MPI})
		SET(MPI_PATH "/opt/nec/ve/mpi/${MPI_VERSION}")
	ENDIF()
	UNSET(NEC_MPI CACHE)

	# If not, try selecting the newest one
	IF(NOT MPI_VERSION)
		_NEC_FIND_FOLDER("/opt/nec/ve/mpi/*" MPI_PATH MPI_VERSION)
	ENDIF()

	IF(NOT MPI_VERSION STREQUAL "")
		MESSAGE(STATUS "Found MPI ${MPI_VERSION}")

		SET(MPI_INCLUDE_PATH		${MPI_PATH}/include)
		SET(MPI_VERSION			${MPI_VERSION}	CACHE STRING "")
		SET(MPI_FOUND			TRUE		CACHE BOOL "")
		SET(MPIEXEC			mpirun		CACHE STRING "Executable for running MPI programs, if provided.")
		SET(MPIEXEC_MAX_NUMPROCS	""		CACHE STRING "Number of MPI processors to utilize. Defaults to the number of processors detected on the host system")
		SET(MPIEXEC_NUMPROC_FLAT	""		CACHE STRING "Flag to pass to MPIEXEC before giving it the number of processors to run on.")
		SET(MPIEXEC_POSTFLAGS		""		CACHE STRING "Flags to pass to MPIEXEC other flags")
		SET(MPIEXEC_PREFLAGS		""		CACHE STRING "Flags to pass to MPIEXEC directly before the executable to run")

		MARK_AS_ADVANCED(MPI_VERSION MPIEXEC MPIEXEC_NUMPROC_FLAT MPIEXEC_MAX_NUMPROCS MPIEXEC_PREFLAGS MPIEXEC_POSTFLAGS MPI_FOUND)

		GET_PROPERTY(MPI_LANGUAGES GLOBAL PROPERTY ENABLED_LANGUAGES)
		FOREACH(LANG ${MPI_LANGUAGES})
			IF(LANG STREQUAL "C" OR LANG STREQUAL "CXX" OR LANG STREQUAL "Fortran")
				GET_FILENAME_COMPONENT(MPI_COMPILER_BIN		${CMAKE_${LANG}_COMPILER}	NAME)

				# General Variables --------------------------------------------
				SET(MPI_${LANG}_COMPILER		${MPI_PATH}/bin/mpi${MPI_COMPILER_BIN}	CACHE FILEPATH "")
				SET(MPI_${LANG}_COMPILE_OPTIONS		${CMAKE_${LANG}_FLAGS}			CACHE STRING "")
				SET(MPI_${LANG}_COMPILE_DEFINITIONS	""					CACHE STRING "")
				SET(MPI_${LANG}_INCLUDE_DIRS		${MPI_INCLUDE_PATH}			CACHE PATH "")
				SET(MPI_${LANG}_LINK_FLAGS		""					CACHE STRING "")
				SET(MPI_${LANG}_LIBRARIES		""					CACHE STRING "")

				MARK_AS_ADVANCED(MPI_${LANG}_COMPILER MPI_${LANG}_COMPILE_OPTIONS MPI_${LANG}_COMPILE_DEFINITIONS MPI_${LANG}_INCLUDE_DIRS MPI_${LANG}_LINK_FLAGS MPI_${LANG}_LIBRARIES)
				UNSET(MPI_COMPILER_BIN)

				# C++ Variables ------------------------------------------------
				IF(LANG STREQUAL "CXX")
					SET(MPI_MPICXX_FOUND TRUE CACHE BOOL "")
					MARK_AS_ADVANCED(MPI_MPICXX_FOUND)
				ENDIF()

				# Fortran Variables --------------------------------------------
				IF(LANG STREQUAL "Fortran")
					SET(MPI_Fortran_HAVE_F77_HEADER TRUE CACHE BOOL "")
					SET(MPI_Fortran_HAVE_F90_MODULE TRUE CACHE BOOL "")
					SET(MPI_Fortran_HAVE_F08_MODULE TRUE CACHE BOOL "")
					LIST(APPEND MPI_Fortran_INCLUDE_DIRS ${MPI_PATH}/lib/ve/module)
					MARK_AS_ADVANCED(MPI_Fortran_HAVE_F77_HEADER MPI_Fortran_HAVE_F90_MODULE MPI_Fortran_HAVE_F08_MODULE)
				ENDIF()

				# Build MPI::MPI_${LANG} target --------------------------------
				IF(NOT TARGET MPI::MPI_${LANG})
					ADD_LIBRARY(MPI::MPI_${LANG} INTERFACE IMPORTED)
				ENDIF()

				SET_PROPERTY(TARGET MPI::MPI_${LANG} PROPERTY INTERFACE_COMPILE_OPTIONS "${MPI_${LANG}_COMPILE_OPTIONS}")
				SET_PROPERTY(TARGET MPI::MPI_${LANG} PROPERTY INTERFACE_COMPILE_DEFINITIONS "${MPI_${LANG}_COMPILE_DEFINITIONS}")

				IF(MPI_${LANG}_LINK_FLAGS)
					SET_PROPERTY(TARGET MPI::MPI_${LANG} PROPERTY INTERFACE_LINK_OPTIONS "SHELL:${MPI_${LANG}_LINK_FLAGS}")
				ENDIF()
				
				IF(MPI_${LANG}_LIBRARIES)
					SET_PROPERTY(TARGET MPI::MPI_${LANG} PROPERTY INTERFACE_LINK_LIBRARIES "${MPI_${LANG}_LIBRARIES}")
				ENDIF()

				SET_PROPERTY(TARGET MPI::MPI_${LANG} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${MPI_${LANG}_INCLUDE_DIRS}")
			ENDIF()
		ENDFOREACH()

		UNSET(MPI_INCLUDE_PATH)
	ENDIF()

	IF(${MPI_FIND_REQUIRED})
		IF(NOT MPI_FOUND)
			MESSAGE(FATAL_ERROR "Unable to detect NEC MPI")
		ENDIF()
	ENDIF()
ENDIF()
