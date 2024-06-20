// Shader for LOD terrain during transition with near terrain
//
// Parameters:

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D LODLandNoise : register(s2);

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
float4 TESR_DebugVar : register(c13);


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

    float4 r0;

    float3 normal = tex2D(NormalMap, IN.BaseUV.xy);
    // float noise = tex2D(LODLandNoise, IN.BaseUV.xy * 1.75);

    float noiseScale = 10000 * TESR_DebugVar.yy;
    float2 noiseUV = fmod(IN.worldpos.xy + 1000000, noiseScale) / noiseScale;
    float noise = tex2D(LODLandNoise, noiseUV).r;

    float3 baseColor = tex2D(BaseMap, IN.BaseUV.xy).xyz;
    baseColor = baseColor * ((noise * 0.8) + 0.55);
    
    float3 lighting = getSunLighting(float3x3(red.xyz, green.xyz, blue.xyz), IN.texcoord_3.xyz, PSLightColor[0].rgb, IN.location.xyz, normal, AmbientColor.rgb);


    // OUT.color_0.rgb = (IN.texcoord_5.w * (IN.texcoord_5.xyz - (baseColor * lighting))) + (baseColor * lighting);
    OUT.color_0.rgb = getFinalColor(lighting, baseColor, white);
    OUT.color_0.a = IN.texcoord_4.x;

    return OUT;
};

// approximately 16 instruction slots used (3 texture, 13 arithmetic)
