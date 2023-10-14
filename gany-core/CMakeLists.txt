cmake_minimum_required(VERSION 3.20)

project(GxAnyCore)

set(TARGET_NAME gany-core)

add_subdirectory(external/rapidjson)

file(GLOB_RECURSE GX_ANY_CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(${TARGET_NAME} STATIC
        ${GX_ANY_CORE_SRC}
        )
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${TARGET_NAME} PUBLIC gany-interface)
target_link_libraries(${TARGET_NAME} PRIVATE rj)

if (MINGW)
    target_link_libraries(${TARGET_NAME} PRIVATE winmm)
endif ()

# Additional dependencies on Unix
if (UNIX AND NOT APPLE AND NOT ANDROID)
    # Threads
    find_package(Threads)
    target_link_libraries(${TARGET_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND NOT ANDROID)
    find_library(RT_LIBRARY rt)
    mark_as_advanced(RT_LIBRARY)
    if (RT_LIBRARY)
        target_link_libraries(${TARGET_NAME} PUBLIC ${RT_LIBRARY})
    endif ()

    find_library(MATH_LIBRARY m)
    mark_as_advanced(MATH_LIBRARY)
    if (MATH_LIBRARY)
        target_link_libraries(${TARGET_NAME} PUBLIC ${MATH_LIBRARY})
    endif ()

    target_link_libraries(${TARGET_NAME} PUBLIC -latomic -ldl)
endif ()

if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    mark_as_advanced(COCOA_LIBRARY)
    target_link_libraries(${TARGET_NAME} PRIVATE ${COCOA_LIBRARY})
endif ()

if (CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-Wa,-mbig-obj" GNU_BIG_OBJ_FLAG_ENABLE)
    message(STATUS GNU_BIG_OBJ_FLAG_ENABLE=${GNU_BIG_OBJ_FLAG_ENABLE})
endif ()

target_compile_options(${TARGET_NAME}
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${GNU_BIG_OBJ_FLAG_ENABLE}>>:-Wa,-mbig-obj>)

## Install
if (GX_LIBS_INSTALL_DIR)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/gx DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-core/include)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gany-core.cmake DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-core)
    install(TARGETS ${TARGET_NAME}
            RUNTIME DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-core/bin
            LIBRARY DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-core/lib
            ARCHIVE DESTINATION ${GX_LIBS_INSTALL_DIR}/gany-core/lib
            )
endif ()