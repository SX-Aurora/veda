SET(VEDA_INSTALL_DEFAULT	"/usr/local/ve")
SET(VEDA_INSTALL_PATH		"veda-${VEDA_VERSION}")
STRING(REGEX MATCH "([0-9]+\.[0-9].+)" VEDA_VERSION_RPM ${CMAKE_VERSION})
SET(CPACK_PACKAGE_NAME		"veda-${VEDA_VERSION_RPM}")

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/aveo/CMakeLists.txt)