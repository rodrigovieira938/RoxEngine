import "res://shaders/internal/roxengine";

namespace urp {
    [SharedUbo("RoxEngine.URP.Globals")]
    cbuffer UrpGlobals
    {
        float4x4 viewMatrix;
        float4x4 projMatrix;
        float4x4 viewProjMatrix;
        float4x4 transformMatrix;
        float3 cameraPosition;
        float3 cameraDirection;
    }
}