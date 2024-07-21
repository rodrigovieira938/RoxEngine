struct VertexStageOutput
{
    float3    color    : CoarseVertex;
    float4    sv_position     : SV_Position;
};

[shader("vertex")]
VertexStageOutput default_vertex_main(float3 position : POSITION)
{
    VertexStageOutput output;

    output.color = float3(1,1,0);;
    output.sv_position = float4(position, 1.0);

    return output;
}

[shader("fragment")]
float4 default_fragment_main(
    float3 color : CoarseVertex) : SV_Target
{
    return float4(color, 1.0);
}