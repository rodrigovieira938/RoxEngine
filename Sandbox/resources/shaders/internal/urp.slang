import "res://shaders/internal/roxengine";

namespace urp {
    [SharedUbo(0, "URP")]
    cbuffer UrpGlobals
    {
        float4x4 viewMatrix;
        float4x4 projMatrix;
        float4x4 viewProjMatrix;
        float3 cameraPosition;
        float3 cameraDirection;
    }
}