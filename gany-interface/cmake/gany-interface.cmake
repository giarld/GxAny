if (NOT GANY_INTERFACE_FIND_PATH)
    message(FATAL_ERROR "Not set: GANY_INTERFACE_FIND_PATH")
endif ()

set(GANY_INTERFACE_INCLUDE_DIR ${GANY_INTERFACE_FIND_PATH}/include)
set(GANY_INTERFACE_BIN_DIR ${GANY_INTERFACE_FIND_PATH}/bin)

add_library(gany-interface INTERFACE)
target_include_directories(gany-interface INTERFACE ${GANY_INTERFACE_INCLUDE_DIR})
