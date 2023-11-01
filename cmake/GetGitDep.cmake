if (NOT GIT_EXECUTABLE)
    find_package(Git QUIET)
    if (NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "Git not found")
    endif ()
endif ()

function(GetGitDependency git_uri dep_name branch)
    if (GIT_DEP_${dep_name}_FOUND)
        message(STATUS "Git dependency ${dep_name} found.")
        return()
    endif ()

    set(GIT_DEP_${dep_name}_FOUND TRUE PARENT_SCOPE)

    set(DEP_PATH ${CMAKE_SOURCE_DIR}/deps)
    set(DEP_SAVE_PATH ${DEP_PATH}/${dep_name})

    file(MAKE_DIRECTORY ${DEP_PATH})

    if (NOT EXISTS ${DEP_SAVE_PATH})
        execute_process(
                COMMAND ${GIT_EXECUTABLE} clone -b ${branch} ${git_uri} ${DEP_SAVE_PATH}
        )
        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} submodule update --init --recursive
        )

        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} log ${branch} -n 1 --pretty=format:"%H"
                OUTPUT_VARIABLE EXT_LOCAL_VER
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        message(STATUS "Get dep \"${dep_name}\" success, branch: ${branch}, hash: ${EXT_LOCAL_VER}")
    else ()
        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} fetch
        )
        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} symbolic-ref --short -q HEAD
                OUTPUT_VARIABLE EXT_BRANCH_NAME
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if (NOT EXT_BRANCH_NAME)
            set(EXT_BRANCH_NAME "unknown")
        endif ()

        if (NOT ${branch} STREQUAL ${EXT_BRANCH_NAME})
            execute_process(
                    COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} checkout ${branch}
            )
        endif ()

        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} log ${branch} -n 1 --pretty=format:"%H"
                OUTPUT_VARIABLE EXT_LOCAL_VER
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
                COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} log remotes/origin/${branch} -n 1 --pretty=format:"%H"
                OUTPUT_VARIABLE EXT_REMOTE_VER
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if (NOT EXT_LOCAL_VER)
            set(EXT_LOCAL_VER "0")
        endif ()
        if (NOT EXT_REMOTE_VER)
            set(EXT_REMOTE_VER "0")
        endif ()

        if (NOT ${EXT_LOCAL_VER} STREQUAL ${EXT_REMOTE_VER})
            execute_process(
                    COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} pull origin ${branch}
            )
            execute_process(
                    COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} submodule sync
            )
            execute_process(
                    COMMAND ${GIT_EXECUTABLE} -C ${DEP_SAVE_PATH} submodule update --init --recursive
            )

            message(STATUS "Update dep ${dep_name} success, branch: ${branch}, hash: ${EXT_REMOTE_VER}")
        endif ()
    endif ()
endfunction(GetGitDependency)