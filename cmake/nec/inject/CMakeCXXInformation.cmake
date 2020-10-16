include(${CMAKE_CURRENT_LIST_DIR}/../VE_CMakeInformation.cmake)
ve_init_information("CXX" "-x c++" "CXX")
SET(CMAKE_CXX_COMPILE_FEATURES)
LIST(APPEND CMAKE_CXX_COMPILE_FEATURES cxx_std_14)
