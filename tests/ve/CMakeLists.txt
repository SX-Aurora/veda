ENABLE_LANGUAGE				(VCXX)

SET(SRC
	${CMAKE_CURRENT_LIST_DIR}/test.vcpp
)

ADD_LIBRARY				(veda_test_ve SHARED ${SRC})
TARGET_COMPILE_OPTIONS	(veda_test_ve PRIVATE -fopenmp)
TARGET_LINK_OPTIONS		(veda_test_ve PRIVATE -Wl,-zdefs)
TARGET_LINK_LIBRARIES	(veda_test_ve veda_device)
SET_TARGET_PROPERTIES	(veda_test_ve PROPERTIES OUTPUT_NAME "veda_test")
INSTALL					(TARGETS veda_test_ve LIBRARY DESTINATION tests)