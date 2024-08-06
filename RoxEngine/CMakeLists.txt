file(GLOB_RECURSE roxengine_src 
    "src/*.cpp" "src/*.h"
)
file(GLOB_RECURSE roxengine_module_src
    "src/*.cxx"
)

add_library(roxengine ${roxengine_src})
target_sources(roxengine
  PUBLIC
    FILE_SET CXX_MODULES FILES
      ${roxengine_module_src}
)

target_include_directories(roxengine PUBLIC src ../Vendor/ ../Vendor/imgui)
target_link_libraries(roxengine PUBLIC glfw glad slang flecs::flecs_static glm)