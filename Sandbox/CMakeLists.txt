file(GLOB_RECURSE sandbox_src 
    "src/*.cpp" "src/*.h"
)
file(GLOB_RECURSE sandbox_module_src
    "src/*.cxx"
)

add_executable(sandbox ${sandbox_src})
target_sources(sandbox
  PUBLIC
    FILE_SET CXX_MODULES FILES
      ${sandbox_module_src}
)
target_link_libraries(sandbox roxengine)