// Shader for LOD terrain during transition with near terrain
//
// Parameters:
sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D LODLandNoise : register(s2);

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
// float4 TESR_DebugVar;


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   BaseMap      texture_0       1
//   NormalMap    texture_1       1
//   LODLandNoise texture_2       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;
    float3 texcoord_3 : TEXCOORD3_centroid;
    float texcoord_4 : TEXCOORD4_centroid;
    float4 texcoord_5 : TEXCOORD5_centroid;
    float4 location : TEXCOORD6;
    float4 worldpos : TEXCOORD7;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};


#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"
// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 eyeDir = -normalize(IN.location.xyz);

    float noise = tex2D(LODLandNoise, IN.BaseUV.xy * TESR_TerrainExtraData.w).r;
    noise = lerp(0, noise, IN.texcoord_4.x);

    float4 normal = tex2D(NormalMap, IN.BaseUV.xy);
    normal.rgb = normalize(expand(normal.rgb));

    float3 baseColor = tex2D(BaseMap, IN.BaseUV.xy).rgb;

    float roughness = saturate(TESR_TerrainData.y * (1 - normal.a));

    float3 lighting = getSunLighting(IN.texcoord_3.xyz, PSLightColor[0].rgb, eyeDir, normal.rgb, AmbientColor.rgb, baseColor, roughness);

    float3 final = lighting;

    OUT.color_0.rgb = lerp(final, final * (0.8 * noise + 0.55), saturate(TESR_TerrainExtraData.z));
    OUT.color_0.a = IN.texcoord_4.x;

    return OUT;
};

// approximately 16 instruction slots used (3 texture, 13 arithmetic)
