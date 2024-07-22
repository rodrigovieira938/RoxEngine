import "res://shaders/internal/roxengine.slang";

[shader("vertex")]
VertexStageOutput basic_vmain(float3 position : POSITION)
{
    VertexStageOutput output;

    output.sv_position = float4(position, 1.0);

    return output;
}

[shader("fragment")]
float4 basic_fmain() : SV_Target
{
    return float4(1,0,0,1);
}