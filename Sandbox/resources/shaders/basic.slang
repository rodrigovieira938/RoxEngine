import "res://shaders/internal/roxengine";


cbuffer Uniforms {
    float3 color;
    float4x4 matrix;
}

[shader("vertex")]
VertexStageOutput basic_vmain(float3 position : POSITION)
{
    VertexStageOutput output;
    output.sv_position = float4(position, 1);
    return output;
}

[shader("fragment")]
float4 basic_fmain() : SV_Target
{
    return float4(1,1,0,1);
}