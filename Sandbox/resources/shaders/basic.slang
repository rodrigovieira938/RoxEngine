import "res://shaders/internal/roxengine";

cbuffer Uniforms {
    float3 color;
    float4x4 matrix;
}

[shader("vertex")]
VertexStageOutput basic_vmain(float3 position : POSITION)
{
    VertexStageOutput output;

    output.sv_position = mul(matrix,float4(position, 1.0));
    
    return output;
}

[shader("fragment")]
float4 basic_fmain() : SV_Target
{
    return float4(color,1);
}