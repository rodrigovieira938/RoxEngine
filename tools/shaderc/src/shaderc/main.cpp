#include "BatchCompiler.hpp"
#include "MaterialBinary.hpp"
#include <filesystem>
 
int main()
{
    
    std::filesystem::current_path(std::filesystem::path(__FILE__).parent_path());//Point the current path to src directory
    BatchCompiler compiler({SLANG_GLSL});
    BatchCompileResult result = compiler.compileFile("shader.slang");
    writeMaterialBinary("shader.bin", result);
    LoadedMaterial material;
    readMaterialBinary("shader.bin", material);
    return 0;
}
 