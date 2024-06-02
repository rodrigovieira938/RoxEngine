file(GLOB_RECURSE sandbox_src 
    "src/*.cpp" "src/*.h"
)

add_executable(sandbox ${sandbox_src})
target_link_libraries(sandbox roxengine)
add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/Vendor/slang/bin/windows-x64/release/slang.dll"
        $<TARGET_FILE_DIR:sandbox>) 
add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/Vendor/slang/bin/windows-x64/release/slang-glslang.dll"
        $<TARGET_FILE_DIR:sandbox>) 