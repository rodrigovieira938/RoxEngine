cmake_minimum_required(VERSION 3.28)
project(RoxEngine VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 20)
include(FetchContent)

macro (FetchContent_Declare)
    _FetchContent_Declare(
        ${ARGV}
        EXCLUDE_FROM_ALL
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
endmacro()

function(fetch_git_deps)
    set(_arg_count ${ARGC})
    # Must be repo/tag pairs
    math(EXPR _remainder "${_arg_count} % 2")
    if(_remainder OR _arg_count EQUAL 0)
        message(WARNING
            "fetch_git_deps() expects arguments in pairs:\n"
            "  <git-repository-url> <git-tag>\n\n"
            "Please specify one dependency per line, like:\n"
            "  fetch_git_deps(\n"
            "    https://github.com/glfw/glfw.git 3.4\n"
            "    https://github.com/g-truc/glm.git 0.9.9.8\n"
            "  )"
        )
        return()
    endif()

    math(EXPR _max "${_arg_count} - 1")
    set(options OPTIONAL FAST)
    set(oneValueArgs DESTINATION RENAME)
    set(multiValueArgs TARGETS CONFIGURATIONS)
    cmake_parse_arguments(arg_my_special_install
        "${options}" "${oneValueArgs}" "${multiValueArgs}"
        ${ARGN}
    )
    foreach(i RANGE 0 ${_max} 2)
        math(EXPR x "${i} + 1")
        list(GET ARGV ${i} repo)
        list(GET ARGV ${x} tag)

        string(REPLACE "/" ";" _parts "${repo}")
        list(GET _parts -1 name)

        if(name MATCHES "\\.git$")
            string(REGEX REPLACE "\\.git$" "" name "${name}")
        endif()

        # Optional sanity check
        if(NOT name)
            message(WARNING "Could not derive project name from repo '${repo}'")
            continue()
        endif()

        message(STATUS "Fetching ${name} (${tag})")

        FetchContent_Declare(
            ${name}
            GIT_REPOSITORY ${repo}
            GIT_TAG        ${tag}
        )
        message(TRACE Fetching ${name})
        FetchContent_MakeAvailable(${name})
        set("${name}_SOURCE_DIR" "${${name}_SOURCE_DIR}" PARENT_SCOPE)
    endforeach()
endfunction()

set(SPDLOG_USE_STD_FORMAT 1)
set(ALINA_BUILD_EXAMPLES OFF)
fetch_git_deps(
    https://github.com/glfw/glfw.git          3.4
    https://github.com/g-truc/glm             1.0.1
    https://github.com/SanderMertens/flecs    v4.1.0
    https://github.com/shader-slang/slang     v2025.12.1
    https://github.com/ocornut/imgui          origin/docking
    https://github.com/gabime/spdlog          v1.15.3
    https://github.com/rodrigovieira938/alina main
)

add_library(imgui-lib STATIC ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
target_include_directories(imgui-lib PUBLIC ${imgui_SOURCE_DIR})

add_subdirectory(RoxEngine)
add_subdirectory(Sandbox)
