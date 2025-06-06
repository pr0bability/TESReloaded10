// Template for fade between terrain and LOD terrain.

#if defined(__INTELLISENSE__)
    #define PS
#endif

#define REVERSED_DEPTH

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"

struct VS_INPUT {
    float4 position : POSITION;
    float4 uv : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 sunDirection : TEXCOORD3;
    float blend : TEXCOORD4;
    float4 fog : TEXCOORD5;
    float3 lPosition : TEXCOORD6;
    float3 eyePosition : TEXCOORD7;
};

#ifdef VS

row_major float4x4 ModelViewProj : register(c0);

float4 FogParam : register(c14);
float3 FogColor : register(c15);
float4 EyePosition : register(c16);
float4 LandBlendParams : register(c19);
float4 LightData : register(c25);

static const float fUVScale = 1.f / 64.f;
static const float fUVScaleQuant = 127.f / 128.f;
static const float fUVOffset = 1.f / 256.f;
static const float fBlendBaseDistance = 9625.59961f;
static const float fBlendScale = 0.000375600968f;

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 posPS = mul(ModelViewProj, IN.position.xyzw);

    OUT.position.xyzw = posPS;

    float2 uv = (IN.uv.xy * fUVScale) + LandBlendParams.xy;
    uv.x = 1 - uv.x;
    
    OUT.uv.xy = (uv.xy * fUVScaleQuant) + fUVOffset;
    OUT.sunDirection.xyz = LightData.xyz;
    
    float2 blendVector = LandBlendParams.zw - IN.position.xy;
    OUT.blend.x = 1 - saturate((fBlendBaseDistance - length(blendVector)) * fBlendScale);
    
    // Fog.
    float3 fogPos = OUT.position.xyz;
    #ifdef REVERSED_DEPTH
        fogPos.z = OUT.position.w - fogPos.z;
    #endif
    float fogStrength = 1 - saturate((FogParam.x - length(fogPos)) / FogParam.y);
    OUT.fog.rgb = FogColor.rgb;
    OUT.fog.a = pow(fogStrength, FogParam.z);

    OUT.lPosition.xyz = IN.position.xyz;
    OUT.eyePosition.xyz = EyePosition.xyz;

    return OUT;
};

#endif  // Vertex shader.

struct PS_INPUT
{
    float2 uv : TEXCOORD0;
    float3 sunDirection : TEXCOORD3_centroid;
    float blend : TEXCOORD4_centroid;
    float4 fog : TEXCOORD5_centroid;
    float3 lPosition : TEXCOORD6_centroid;
    float3 eyePosition : TEXCOORD7_centroid;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

#ifdef PS

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D LODLandNoise : register(s2);

float4 AmbientColor : register(c1);
float4 PSLightColor : register(c3);

float4 LandLODSpec : register(c38);

static const float fNoiseScale = 0.8f;
static const float fNoiseOffset = 0.55f;

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;

    float3 eyeDir = normalize(IN.eyePosition.xyz - IN.lPosition.xyz);

    float noise = tex2D(LODLandNoise, IN.uv.xy * TESR_TerrainExtraData.w).r;

    float4 normal = tex2D(NormalMap, IN.uv.xy);
    normal.rgb = normalize(expand(normal.rgb));
    normal.a *= LandLODSpec.x > 0.0f ? 1.0f : 0.0f;

    float3 baseColor = tex2D(BaseMap, IN.uv.xy).rgb;

    float3 lighting = getSunLighting(IN.sunDirection.xyz, PSLightColor.rgb, eyeDir, normal.xyz, AmbientColor.rgb, baseColor, normal.a, LandLODSpec.x);

    float3 final = lighting;
    final = lerp(final, final * (noise * fNoiseScale + fNoiseOffset), saturate(TESR_TerrainExtraData.z)); // Apply noise.
    final = lerp(final, IN.fog.rgb, IN.fog.a); // Apply fog.

    OUT.color_0.rgb = final;
    OUT.color_0.a = IN.blend.x;

    return OUT;
};

#endif  // Pixel shader.
