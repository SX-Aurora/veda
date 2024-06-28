SET(AVEO_REPO		"https://github.com/SX-Aurora/aveo")
SET(AVEO_PATH		${CMAKE_CURRENT_BINARY_DIR}/aveo)
SET(AVEO_GIT_TAG	"master")

if(CMAKE_NCC_PATH)
	set(AVEO_NCC ${CMAKE_NCC_PATH})
else()
	set(AVEO_NCC "/opt/nec/ve")
endif()

if(CMAKE_NFORT_PATH)
	set(AVEO_NFORT ${CMAKE_NFORT_PATH})
else()
	set(AVEO_NFORT "/opt/nec/ve")
endif()

SET(libexecdir "@libexecdir@")
SET(velibdir "@velibdir@")
CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/aveo.patch ${CMAKE_CURRENT_BINARY_DIR}/aveo.patch @ONLY)
UNSET(libexecdir)
UNSET(velibdir)

INCLUDE(ExternalProject)

FUNCTION(MAKE_AVEO VE_ARCH)
	ExternalProject_Add(aveo_${VE_ARCH}
		BINARY_DIR		${AVEO_PATH}/${VE_ARCH}/src
		BUILD_COMMAND		make "VE_ARCH=${VE_ARCH}"
		CONFIGURE_COMMAND	""
		GIT_REPOSITORY		${AVEO_REPO}
		GIT_SHALLOW		FALSE
		GIT_TAG			${AVEO_GIT_TAG}
		INSTALL_COMMAND		make "VE_ARCH=${VE_ARCH}" install
		INSTALL_DIR		""
		PATCH_COMMAND		git checkout . && git apply 
						${CMAKE_CURRENT_BINARY_DIR}/aveo.patch
						${CMAKE_CURRENT_LIST_DIR}/aveo.patch2
		PREFIX			${AVEO_PATH}/${VE_ARCH}
		SOURCE_DIR		${AVEO_PATH}/${VE_ARCH}/src
		STAMP_DIR		${AVEO_PATH}/${VE_ARCH}/stamp
		TMP_DIR			${AVEO_PATH}/${VE_ARCH}/tmp
		UPDATE_DISCONNECTED	TRUE
	)
ENDFUNCTION()

MAKE_AVEO(ve1)
MAKE_AVEO(ve3)

SET(AVEO_INCLUDE_DIRS	"${AVEO_PATH}/ve1/src/src/")
SET(AVEO_LIBRARY	"${AVEO_PATH}/ve1/src/install/lib64/libveo.so.1.0.0")

INSTALL(FILES ${AVEO_LIBRARY} DESTINATION ${VEDA_INSTALL_PATH}/lib64 RENAME libveo.so.1)
INSTALL(FILES
	"${AVEO_PATH}/ve1/src/install/libexec/aveorun_ve1"
	"${AVEO_PATH}/ve1/src/install/libexec/aveorun-ftrace_ve1"
	"${AVEO_PATH}/ve3/src/install/libexec/aveorun_ve3"
	"${AVEO_PATH}/ve3/src/install/libexec/aveorun-ftrace_ve3"
PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE DESTINATION ${VEDA_INSTALL_PATH}/libexec)

ILLYRIAN_INSTALL_SYMLINK(libveo.so.1 libveo.so ${VEDA_DIST_TYPE} DESTINATION ${VEDA_INSTALL_PATH}/lib64/)

SET(AVEO_LIBRARIES ${AVEO_LIBRARY})
