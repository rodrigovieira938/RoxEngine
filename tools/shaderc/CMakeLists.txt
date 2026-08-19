add_executable(shaderc_test src/shaderc/main.cpp)
target_link_libraries(shaderc_test PRIVATE slang)
add_library(shaderc INTERFACE)
target_include_directories(shaderc INTERFACE src)