file(GLOB_RECURSE roxengine_src 
    "src/*.cpp" "src/*.h"
)

add_library(roxengine ${roxengine_src})
target_link_libraries(roxengine PUBLIC glfw glad)