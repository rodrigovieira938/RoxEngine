module roxengine;

public struct VertexStageOutput
{
    public float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexStageOutput default_vertex_main(float3 position : POSITION)
{
    VertexStageOutput output;

    output.sv_position = float4(position, 1.0);

    return output;
}

[shader("fragment")]
float4 default_fragment_main(
    float3 color : CoarseVertex) : SV_Target
{
    //TODO: output a null texture like in minecraft
    return float4(1,1,0, 1);
}