INSTALL(FILES
	${CMAKE_CURRENT_LIST_DIR}/CMakeCInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeCXXInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineCCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineCXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineFortranCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeFortranInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestCCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestCXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestFortranCompiler.cmake
	DESTINATION cmake/nec/inject)
