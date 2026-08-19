#include "BatchCompiler.hpp"
#include "MaterialBinary.hpp"
#include <filesystem>
#include <iostream>
 
static const char* sample = R"SRC(
// @material Metal_Standard
// @domain surface
// @permutation USE_NORMAL_MAP = 0, 1
// @permutation USE_EMISSION   = 0, 1
// @blend opaque
// @cull back
// @depth_test less
// @depth_write true
// @tag category=metal transparent=false lowpoly
// this is just a normal comment, not a directive
import MaterialCommon;
 
@permutation QUALITY = LOW, MEDIUM, HIGH
struct MAT
{
    float3 baseColor;
};
)SRC";
 
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
 