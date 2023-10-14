if (NOT GANY_CORE_FIND_PATH)
    message(FATAL_ERROR "Not set: GANY_CORE_FIND_PATH")
endif ()

if (WIN32)
    set(LINK_LIBRARY_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(LINK_LIBRARY_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
else ()
    set(LINK_LIBRARY_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(LINK_LIBRARY_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
endif ()

set(GANY_CORE_INCLUDE_DIR ${GANY_CORE_FIND_PATH}/include)
set(GANY_CORE_LIB_DIR ${GANY_CORE_FIND_PATH}/lib)

add_library(gany-core INTERFACE)
target_include_directories(gany-core INTERFACE ${GANY_CORE_INCLUDE_DIR})
target_link_libraries(gany-core INTERFACE
        ${GANY_CORE_LIB_DIR}/${LINK_LIBRARY_PREFIX}gany-core${LINK_LIBRARY_SUFFIX})

if (UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(Threads)
    target_link_libraries(gany-core INTERFACE ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND NOT ANDROID)
    find_library(RT_LIBRARY rt)
    mark_as_advanced(RT_LIBRARY)
    if (RT_LIBRARY)
        target_link_libraries(gany-core INTERFACE ${RT_LIBRARY})
    endif ()

    find_library(MATH_LIBRARY m)
    mark_as_advanced(MATH_LIBRARY)
    if (MATH_LIBRARY)
        target_link_libraries(gany-core INTERFACE ${MATH_LIBRARY})
    endif ()

    target_link_libraries(gany-core INTERFACE -latomic -ldl)
endif ()
