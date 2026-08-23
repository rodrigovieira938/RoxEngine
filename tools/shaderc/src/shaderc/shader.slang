// @material PBRLit
// @domain surface

// @blend alpha_blend
// @cull back
// @depth_test less_equal
// @depth_write true

// @permutation USE_NORMAL_MAP
// @permutation QUALITY = LOW, MEDIUM, HIGH
// @permutation SHADOW_MODE = OFF, HARD, SOFT

// @tag category=pbr
// @tag render_queue=transparent
// @tag receives_shadows=true casts_shadows=true
// @tag instanced

#define LOW 0
#define MEDIUM 1
#define HIGH 2
#ifndef QUALITY
#define QUALITY MEDIUM
#endif
#ifndef SHADOW_MODE
#define SHADOW_MODE HARD
#endif

Texture2D<float4> BaseColorTexture;
Texture2D<float4> NormalTexture;
SamplerState LinearSampler;

cbuffer MaterialParams
{
    float4 BaseColor;
    float Metallic;
    float Roughness;
}

struct VertexInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float4 tangent  : TANGENT;
    float2 uv       : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
};

cbuffer Globals
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 viewProjMatrix;
    float4x4 transformMatrix;
    float4x4 transformsMatrix[4];
    float3 cameraPosition;
    float3 cameraDirection;
}

[shader("vertex")]
VertexOutput vsMain(VertexInput input)
{
    VertexOutput output;

    float4 worldPos = mul(transformMatrix, float4(input.position, 1.0));
    
    output.position = mul(viewProjMatrix, worldPos);
    output.normal = input.normal;
    output.uv = input.uv;

    return output;
}

[shader("fragment")]
float4 psMain(VertexOutput input) : SV_Target
{
    float4 color = BaseColorTexture.Sample(LinearSampler, input.uv);

#if USE_NORMAL_MAP
    // Sample normal map...
    float3 normal =
        NormalTexture.Sample(LinearSampler, input.uv).xyz * 2.0 - 1.0;
#endif

#if QUALITY == HIGH
    color.rgb *= 1.20;
#elif QUALITY == MEDIUM
    color.rgb *= 1.10;
#else 
    color.rgb *= 1.3;
#endif

/*#if SHADOW_MODE == HARD
    color.a *= 0.8;
#elif SHADOW_MODE == SOFT
    color.a *= 0.6;
#endif*/

    return color * BaseColor;
}