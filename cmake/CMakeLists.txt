INSTALL(FILES
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVCCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVCXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVCCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVCXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVCInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVCXXInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindVE.cmake
	${CMAKE_CURRENT_LIST_DIR}/VE_CMakeCompiler.cmake.in
	${CMAKE_CURRENT_LIST_DIR}/VE_CMakeDetermineCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/VE_CMakeInformation.cmake
	DESTINATION cmake)
