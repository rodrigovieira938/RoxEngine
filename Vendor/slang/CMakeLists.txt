add_library(slang INTERFACE)
target_include_directories(slang INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(slang INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/bin/windows-x64/release/slang.lib")