// Template for LOD terrain shaders.

#if defined(__INTELLISENSE__)
    #define PS
#endif

#define REVERSED_DEPTH

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"

struct VS_INPUT {
    float4 position : POSITION;
    float4 uv : TEXCOORD0;
    float geomorphHeight : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 fog : COLOR1;
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 sunDirection : TEXCOORD1;
    float3 lPosition : TEXCOORD2;
    float3 eyePosition : TEXCOORD3;
};

#ifdef VS

row_major float4x4 ModelViewProj : register(c0);
row_major float4x4 ObjToCubeSpace : register(c8);

float4 HighDetailRange : register(c12);
float4 FogParam : register(c14);
float3 FogColor : register(c15);
float4 EyePosition : register(c16);
float4 GeomorphParams : register(c19);
float4 LightData : register(c25);

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 posLS = IN.position;
    float4 geomorphPos = IN.position;
    geomorphPos.z = lerp(IN.geomorphHeight.x, IN.position.z, GeomorphParams.x);

    float q0 = (abs(dot(ObjToCubeSpace[1].xyzw, geomorphPos.xyzw) - HighDetailRange.y) < HighDetailRange.w ? 1.0 : 0.0);
    float q1 = (abs(dot(ObjToCubeSpace[0].xyzw, geomorphPos.xyzw) - HighDetailRange.x) < HighDetailRange.z ? 1.0 : 0.0);

    posLS.z = geomorphPos.z - ((q0.x * q1.x) * GeomorphParams.y);
    float4 posPS = mul(ModelViewProj, posLS.xyzw);

    OUT.position.xyzw = posPS.xyzw;
    OUT.uv.xy = IN.uv.xy;
    OUT.sunDirection = LightData.xyz;
    
    // Fog.
    float3 fogPos = OUT.position.xyz;
    #ifdef REVERSED_DEPTH
        fogPos.z = OUT.position.w - fogPos.z;
    #endif
    float fogStrength = 1 - saturate((FogParam.x - length(fogPos)) / FogParam.y);
    OUT.fog.rgb = FogColor.rgb;
    OUT.fog.a = pow(fogStrength, FogParam.z);

    OUT.lPosition.xyz = posLS.xyz;
    OUT.eyePosition.xyz = EyePosition.xyz;

    return OUT;
};

#endif  // Vertex shader.

struct PS_INPUT
{
    float4 fog : COLOR1;
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 sunDirection : TEXCOORD1_centroid;
    float3 lPosition : TEXCOORD2_centroid;
    float3 eyePosition : TEXCOORD3_centroid;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

#ifdef PS

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D LODParentTex : register(s4);
sampler2D LODParentNormals : register(s6);
sampler2D LODLandNoise : register(s7);

float4 AmbientColor : register(c1);
float4 PSLightColor : register(c3);
float4 LODTexParams : register(c31);

float4 LandLODSpec : register(c38);

static const float fUVScale = 1.f / 128.f;
static const float fUVScaleQuant = 127.f / 128.f;
static const float fUVOffset = 1.f / 256.f;
static const float fNoiseScale = 0.8f;
static const float fNoiseOffset = 0.55f;

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;

    float4 normal = tex2D(NormalMap, IN.uv);
    float4 parentNormal = tex2D(LODParentNormals, (IN.uv * 0.5f) + LODTexParams.xy);

    float noise = tex2D(LODLandNoise, IN.uv * TESR_TerrainExtraData.w).r;

    normal.rgba = LODTexParams.w >= 1 ? normal.rgba : lerp(parentNormal, normal.rgba, LODTexParams.w);
    normal.rgb = expand(normal.rgb);
    normal.rgb = normalize(normal.rgb);
    normal.a *= LandLODSpec.x > 0.0f ? 1.0f : 0.0f;

    float2 uv = (IN.uv * fUVScaleQuant) + fUVOffset;
    float3 parentColor = tex2D(LODParentTex, (0.5f * uv) + lerp(LODTexParams.xy, 0.25f, fUVScale)).rgb;
    float3 baseColor = tex2D(BaseMap, uv).rgb;
    
    baseColor = LODTexParams.w >= 1 ? baseColor : lerp(parentColor, baseColor, LODTexParams.w);
    
    float3 eyeDir = normalize(IN.eyePosition.xyz - IN.lPosition.xyz);
    
    float3 lighting = getSunLighting(IN.sunDirection.xyz, PSLightColor.rgb, eyeDir, normal.xyz, AmbientColor.rgb, baseColor, normal.a, LandLODSpec.x);

    float3 final = lighting;
    final = lerp(final, final * (0.8 * noise + 0.55), saturate(TESR_TerrainExtraData.z)); // Apply noise.
    final = lerp(final, IN.fog.rgb, IN.fog.a);

    OUT.color_0.rgb = final;
    OUT.color_0.a = 1;

    return OUT;
};

#endif  // Pixel shader.
