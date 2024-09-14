//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
sampler2D LODLandNoise : register(s7);
sampler2D LODParentNormals : register(s6);
sampler2D LODParentTex : register(s4);
float4 LODTexParams : register(c31);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10] : register(c3);


// Registers:
//
//   Name             Reg   Size
//   ---------------- ----- ----
//   AmbientColor     const_1       1
//   PSLightColor[0]     const_3       1
//   LODTexParams     const_31      1
//   BaseMap          texture_0       1
//   NormalMap        texture_1       1
//   LODParentTex     texture_4       1
//   LODParentNormals texture_6       1
//   LODLandNoise     texture_7       1
//


// Structures:

struct VS_INPUT {
    float2 NormalUV : TEXCOORD0;
    float4 color_1 : COLOR1;
    float3 texcoord_1 : TEXCOORD1_centroid;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

#include "includes/Helpers.hlsl"
// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 r0 = LODTexParams.xyw;

    float3 normal = tex2D(NormalMap, IN.NormalUV).xyz;
    float noise = tex2D(LODLandNoise, IN.NormalUV * 1.75).r;
    float3 parentNormal = tex2D(LODParentNormals, (IN.NormalUV * 0.5) + r0.xy).xyz;
    
    normal = r0.z >= 1 ? normal : lerp(parentNormal, normal, LODTexParams.w);

    float lighting = shades(expand(normal), IN.texcoord_1.xyz);
    float3 q4 = max((lighting * PSLightColor[0].rgb) + AmbientColor.rgb, 0);
    
    float2 q1 = (IN.NormalUV * 0.9921875) + (1.0 / 256);
    float3 r1 = tex2D(LODParentTex, (0.5 * q1) + lerp(r0.xy, 0.25, (1.0 / 128))).rgb;
    float3 r2 = tex2D(BaseMap, q1).rgb;
    
    float3 q5 = (r0.z >= 1 ? r2 : lerp(r1, r2, LODTexParams.w)) * ((noise * 0.8) + 0.55);


    OUT.color_0.rgb = (IN.color_1.a * (IN.color_1.rgb - (q5 * q4))) + (q5 * q4);

    OUT.color_0.rgb *= green.rgb;


    OUT.color_0.a = 1;

    return OUT;
};

// approximately 29 instruction slots used (5 texture, 24 arithmetic)
