file(GLOB_RECURSE sandbox_src 
    "src/*.cpp" "src/*.h"
)

add_executable(Sandbox ${sandbox_src})
target_link_libraries(Sandbox RoxEngine)