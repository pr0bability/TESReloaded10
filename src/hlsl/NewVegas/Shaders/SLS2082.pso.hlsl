// Shader for LOD terrain during transition with near terrain
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
sampler2D LODLandNoise : register(s2);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10] : register(c3);


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
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};


#include "includes/Helpers.hlsl"
// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r0;

    float3 noxel0 = tex2D(NormalMap, IN.BaseUV.xy);
    float noise = tex2D(LODLandNoise, IN.BaseUV.xy * 1.75);
    float3 r1 = tex2D(BaseMap, IN.BaseUV.xy).xyz;
    
    float3 q4 = r1 * ((noise * 0.8) + 0.55);
    
    float3 q1 = (shades(expand(noxel0), IN.texcoord_3.xyz) * PSLightColor[0].rgb) + AmbientColor.rgb;
    
    OUT.color_0.rgb = (IN.texcoord_5.w * (IN.texcoord_5.xyz - (q4 * q1))) + (q4 * q1);

    OUT.color_0.a = IN.texcoord_4.x;

    return OUT;
};

// approximately 16 instruction slots used (3 texture, 13 arithmetic)
