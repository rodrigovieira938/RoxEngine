#include "MaterialCommentParser.hpp"
 
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
 
struct MAT
{
    float3 baseColor;
};
)SRC";
 
int main()
{
    MaterialFileMeta meta = parseMaterialComments(sample);
    return 0;
}
 