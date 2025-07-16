file(GLOB_RECURSE sandbox_src 
    "src/*.cpp" "src/*.h"
)

add_executable(sandbox ${sandbox_src})
target_link_libraries(sandbox PRIVATE roxengine)