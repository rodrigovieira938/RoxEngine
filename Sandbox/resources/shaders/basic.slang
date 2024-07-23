import "res://shaders/internal/roxengine";

cbuffer Uniforms {
    int input1;
    float color[3];
}

[shader("vertex")]
VertexStageOutput basic_vmain(float3 position : POSITION)
{
    VertexStageOutput output;

    output.sv_position = float4(position.x * input1, position.yz, 1.0);
    
    return output;
}

[shader("fragment")]
float4 basic_fmain() : SV_Target
{
    return float4(color[0],color[1],color[2],1);
}