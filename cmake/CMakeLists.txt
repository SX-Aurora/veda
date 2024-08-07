INCLUDE(${CMAKE_CURRENT_LIST_DIR}/nec/CMakeLists.txt)
CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/InjectVE.in.cmake ${CMAKE_CURRENT_BINARY_DIR}/InjectVE.cmake @ONLY)

INSTALL(FILES
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVEDA_CCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVEDA_CXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeDetermineVEDA_FortranCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVEDA_CCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVEDA_CXXCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeTestVEDA_FortranCompiler.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVEDA_CInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVEDA_CXXInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/CMakeVEDA_FortranInformation.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindMPI.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindASL.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindBLAS.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindLAPACK.cmake
	${CMAKE_CURRENT_LIST_DIR}/FindNLC.cmake
	${CMAKE_CURRENT_BINARY_DIR}/InjectVE.cmake
DESTINATION ${VEDA_INSTALL_PATH}/cmake)

ILLYRIAN_INSTALL_FIND_BEGIN	(VEDA PYTHON_NAME veda FILE "include/veda.h" PATHS "/usr/local/ve/veda/" "/opt/nec/ve/share/veda")
ILLYRIAN_INSTALL_FIND_REQUIRED	(LIBRARY VEDA LIBRARY "libveda.so" "libveda.a" PATHS "\${VEDA_DIR}/lib64" "\${VEDA_DIR}/../../veos/lib64")
ILLYRIAN_INSTALL_FIND_OPTIONAL	(LIBRARY VERA LIBRARY "libvera.so" "libvera.a" PATHS "\${VEDA_DIR}/lib64" "\${VEDA_DIR}/../../veos/lib64") # Optional Feature
ILLYRIAN_INSTALL_FIND_REQUIRED	(FILE VEDA DEVICE_LIBRARY "libveda.vso" PATHS "\${VEDA_DIR}/libve" "\${VEDA_DIR}/../../veos/lib64")
ILLYRIAN_INSTALL_FIND_REQUIRED	(FILE VEDA DEVICE3_LIBRARY "libveda.vso3" PATHS "\${VEDA_DIR}/libve" "\${VEDA_DIR}/../../veos/lib64")
ILLYRIAN_INSTALL_FIND_SET	(VEDA INCLUDE_DIRS "\${VEDA_DIR}/include" CACHE STRING "VEDA dir")
ILLYRIAN_INSTALL_FIND_END	(VEDA
	VERSION_FILE "\${VEDA_INCLUDE_DIRS}/veda/version.h"
	VERSION_REGEX "\\\"([0-9\.]+)\\\""
	DESTINATION ${VEDA_INSTALL_PATH}/cmake
)