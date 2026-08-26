import "res://shaders/internal/roxengine";
import "res://shaders/internal/urp";


cbuffer Uniforms {
    float3 color;
};

[shader("vertex")]
VertexStageOutput basic_vmain(float3 position : POSITION, float4x4 transformMatrix : INSTANCE_DATA)
{
    VertexStageOutput output;
    // float4 worldPos = mul(transformMatrix, float4(position, 1.0));
    // output.sv_position = mul(urp::viewProjMatrix, worldPos);
    output.sv_position = float4(position.x, position.y, position.z, 1.0);
    return output;
}

[shader("fragment")]
float4 basic_fmain() : SV_Target
{
    return float4(color, 1);
}