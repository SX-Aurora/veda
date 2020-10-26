INCLUDE(${CMAKE_CURRENT_LIST_DIR}/nec/CMakeLists.txt)
CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/InjectVE.in.cmake ${CMAKE_CURRENT_BINARY_DIR}/InjectVE.cmake @ONLY)

INSTALL(FILES
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVEDA_CCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVEDA_CXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVEDA_FortranCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVE_CCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVE_CXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVE_FortranCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVEDA_CCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVEDA_CXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVEDA_FortranCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVE_CCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVE_CXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVE_FortranCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVEDA_CInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVEDA_CXXInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVEDA_FortranInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVE_CInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVE_CXXInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVE_FortranInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindMPI.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindASL.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindBLAS.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindLAPACK.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindNLC.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindVE.cmake
	${CMAKE_CURRENT_BINARY_DIR}/InjectVE.cmake
	DESTINATION cmake)
