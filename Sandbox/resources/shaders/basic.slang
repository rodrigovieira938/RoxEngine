import "res://shaders/internal/roxengine";


cbuffer Uniforms {
    float3 color;
    float4x4 matrix;
}

cbuffer TestBuffer{
    float     scalarFloat;
    int       scalarInt;
    bool      scalarBool;
    float2    vector2;
    float3    vector3;
    float4    vector4;
    float3x3  matrix3x3;
    float4x4  matrix4x4;
    float     array1D[5];
    float    array2D[3][4];
    float    array3D[2][3][4];
};


[shader("vertex")]
VertexStageOutput basic_vmain(float3 position : POSITION)
{
    VertexStageOutput output;
    output.sv_position = mul(matrix,float4(position, scalarInt));
    return output;
}

[shader("fragment")]
float4 basic_fmain() : SV_Target
{
    return float4(color,1);
}