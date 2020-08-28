INCLUDE(${CMAKE_CURRENT_LIST_DIR}/inject/CMakeLists.txt)

INSTALL(FILES
	${CMAKE_CURRENT_LIST_DIR}/VE_CMakeCompiler.cmake.in
	${CMAKE_CURRENT_LIST_DIR}/VE_CMakeDetermineCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/VE_CMakeInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/VE_Helper.cmake
	DESTINATION cmake/nec)
