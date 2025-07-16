cmake_minimum_required(VERSION 3.28)
project(RoxEngine VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 20)
include(FetchContent)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        658f48e72aee3c6582e80b05ac0f8787a64fe6bb #v2.0.6
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  cmake
)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs
    GIT_TAG v4.1.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    slang
    GIT_REPOSITORY https://github.com/shader-slang/slang
    GIT_TAG v2025.12.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG origin/docking
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.15.3
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
set(SPDLOG_USE_STD_FORMAT 1)
FetchContent_MakeAvailable(glad glfw glm flecs slang imgui spdlog)

glad_add_library(glad STATIC REPRODUCIBLE LOADER API gl:core=4.4)
add_library(imgui-lib STATIC ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
target_include_directories(imgui-lib PUBLIC ${imgui_SOURCE_DIR})

add_subdirectory(RoxEngine)
add_subdirectory(Sandbox)
