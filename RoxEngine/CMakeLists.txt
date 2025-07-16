file(GLOB_RECURSE roxengine_src 
    "src/*.cpp" "src/*.h"
)

add_library(roxengine ${roxengine_src})
target_include_directories(roxengine PUBLIC src)
target_link_libraries(roxengine PUBLIC glfw glad slang flecs::flecs_static glm imgui-lib spdlog)