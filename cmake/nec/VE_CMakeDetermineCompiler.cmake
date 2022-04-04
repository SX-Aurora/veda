SET(VEDA_NEC_PATH ${CMAKE_CURRENT_LIST_DIR})
macro(ve_init_compiler LANG COMPILER EXTENSIONS PREFERENCE)
	set(CMAKE_VE_PATH "/opt/nec/ve/bin")

	if(NOT CMAKE_${LANG}_COMPILER)
		find_file(CMAKE_${LANG}_COMPILER ${COMPILER} PATHS ${CMAKE_VE_PATH})
		find_file(CMAKE_${LANG}_COMPILER_AR nar PATHS ${CMAKE_VE_PATH})
		find_file(CMAKE_${LANG}_COMPILER_RANLIB nranlib PATHS ${CMAKE_VE_PATH})
		mark_as_advanced(CMAKE_${LANG}_COMPILER CMAKE_${LANG}_COMPILER_AR CMAKE_${LANG}_COMPILER_RANLIB)

		execute_process(COMMAND ${CMAKE_${LANG}_COMPILER} "--version" ERROR_VARIABLE RES)
		string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" RES ${RES})
		message(STATUS "The ${LANG} compiler identification is NCC ${RES}")
		set(CMAKE_VE_COMPILER_ID "NCC ${RES}")
		unset(RES)
	endif()

	# Macro Args are no Variables!
	set(LANG ${LANG})
	set(PREFERENCE ${PREFERENCE})
	set(EXTENSIONS ${EXTENSIONS})
	set(FLAGS "")
	set(CMAKE_VE_COMPILER ${CMAKE_${LANG}_COMPILER})
	set(CMAKE_VE_COMPILER_AR ${CMAKE_${LANG}_COMPILER_AR})
	set(CMAKE_VE_COMPILER_RANLIB ${CMAKE_${LANG}_COMPILER_RANLIB})

	if(CMAKE_CXX_STANDARD_REQUIRED)
		if(CMAKE_CXX_STANDARD EQUAL 11)
			list(APPEND FLAGS "-std=c++11")
		elseif(CMAKE_C_STANDARD EQUAL 13)
			list(APPEND FLAGS "-std=c++13")
		elseif(CMAKE_C_STANDARD EQUAL 17)
			list(APPEND FLAGS "-std=c++17")
		else()
			message(FATAL_ERROR "Unsupported CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
		endif()
	endif()

	configure_file(${VEDA_NEC_PATH}/VE_CMakeCompiler.cmake.in
		${CMAKE_PLATFORM_INFO_DIR}/CMake${LANG}Compiler.cmake
		@ONLY
	)

	set(CMAKE_${LANG}_COMPILER_ENV_VAR "${LANG}")

	unset(LANG)
	unset(EXTENSIONS)
	unset(PREFERENCE)
	unset(COMPILER)
	unset(CMAKE_VE_COMPILER)
	unset(CMAKE_VE_COMPILER_AR)
	unset(CMAKE_VE_COMPILER_RANLIB)
endmacro()
