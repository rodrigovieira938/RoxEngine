#include "BatchCompiler.hpp"
#include "MaterialBinary.hpp"
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
    BatchCompiler compiler({SLANG_GLSL});
    BatchCompileResult result = compiler.compileFile("/home/rv/dev/cpp/roxengine/tools/shaderc/src/shader.slang");
    writeMaterialBinary("/home/rv/dev/cpp/roxengine/tools/shaderc/src/shader.bin", result);
    return 0;
}
 