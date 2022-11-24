LIST(APPEND VEDA_SRC
	${CMAKE_CURRENT_LIST_DIR}/Args.cpp
	${CMAKE_CURRENT_LIST_DIR}/CFunction.cpp
	${CMAKE_CURRENT_LIST_DIR}/Device.cpp
	${CMAKE_CURRENT_LIST_DIR}/Exception.cpp
	${CMAKE_CURRENT_LIST_DIR}/Future.cpp
	${CMAKE_CURRENT_LIST_DIR}/HostFunction.cpp
	${CMAKE_CURRENT_LIST_DIR}/Profiler.cpp
	${CMAKE_CURRENT_LIST_DIR}/Ptr.cpp
	${CMAKE_CURRENT_LIST_DIR}/static.cpp
)

## Install Files ---------------------------------------------------------------
INSTALL(FILES
	${CMAKE_CURRENT_LIST_DIR}/Args.h
	${CMAKE_CURRENT_LIST_DIR}/CFunction.h
	${CMAKE_CURRENT_LIST_DIR}/Device.h
	${CMAKE_CURRENT_LIST_DIR}/Exception.h
	${CMAKE_CURRENT_LIST_DIR}/Future.h
	${CMAKE_CURRENT_LIST_DIR}/HostFunction.h
	${CMAKE_CURRENT_LIST_DIR}/Profiler.h
	${CMAKE_CURRENT_LIST_DIR}/Ptr.h
	${CMAKE_CURRENT_LIST_DIR}/Template.h
	${CMAKE_CURRENT_LIST_DIR}/Template_impl.h
	${CMAKE_CURRENT_LIST_DIR}/Template_internal.h
	${CMAKE_CURRENT_LIST_DIR}/Template_types.h
	${CMAKE_CURRENT_LIST_DIR}/api.h
	${CMAKE_CURRENT_LIST_DIR}/static.h
	${CMAKE_CURRENT_LIST_DIR}/types.h
DESTINATION ${VEDA_INSTALL_PATH}/include/veda/cpp)
