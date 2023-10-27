cmake_minimum_required(VERSION 3.20)
project(GxAny)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_SIZEOF_VOID_P)
    if (CMAKE_CL_64)
        set(CMAKE_SIZEOF_VOID_P 8)
    else ()
        set(CMAKE_SIZEOF_VOID_P 4)
    endif ()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

    if (BUILD_MODE_MT)
        add_compile_options(
                $<$<CONFIG:>:/MT>
                $<$<CONFIG:Debug>:/MTd>
                $<$<CONFIG:Release>:/MT>
        )
    else ()
        add_compile_options(
                $<$<CONFIG:>:/MD>
                $<$<CONFIG:Debug>:/MDd>
                $<$<CONFIG:Release>:/MD>
        )
    endif ()
endif ()


if (NOT T_EXECUTABLE_OUTPUT_PATH)
    set(T_EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
endif ()

if (NOT T_LIBRARY_OUTPUT_PATH)
    set(T_LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${T_EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${T_EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${T_EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${T_EXECUTABLE_OUTPUT_PATH})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${T_LIBRARY_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${T_LIBRARY_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${T_LIBRARY_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${T_LIBRARY_OUTPUT_PATH})

if (NOT WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${T_LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${T_LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${T_LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${T_LIBRARY_OUTPUT_PATH})
endif ()


option(GANY_ENABLE_TEST "Enable test." ON)

if (NOT GX_LIBS_INSTALL_DIR)
    set(GX_LIBS_INSTALL_DIR ${CMAKE_BINARY_DIR}/dev)
endif ()

add_subdirectory(gany-interface)
add_subdirectory(gany-core)
add_subdirectory(gany-c-api)

if (GANY_ENABLE_TEST)
    include(cmake/GetGitDep.cmake)

    GetGitDependency(git@github.com:google/googletest.git ${CMAKE_CURRENT_SOURCE_DIR}/deps gtest v1.14.0)
    add_subdirectory(deps/gtest)

    add_subdirectory(test)
endif ()
