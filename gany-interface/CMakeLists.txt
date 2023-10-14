cmake_minimum_required(VERSION 3.20)

project(GxAnyInterface)

set(TARGET_NAME gany-interface)

add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

### Install
if (GX_LIBS_INSTALL_DIR)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/gx DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-interface/include)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gany-interface.cmake DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-interface)
    install(TARGETS ${TARGET_NAME}
            RUNTIME DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-interface/bin
            LIBRARY DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-interface/lib
            ARCHIVE DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-interface/lib
    )
endif ()
