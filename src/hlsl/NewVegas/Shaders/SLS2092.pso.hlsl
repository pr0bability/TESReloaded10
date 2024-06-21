//
//
// Parameters:
sampler2D BaseMap[7] : register(s0);
sampler2D NormalMap[7] : register(s7);

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
float4 PSLightDir : register(c18);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   PSLightDir   const_18      1
//   BaseMap      texture_0       1
//   NormalMap    texture_7       1
//


// Structures:

struct VS_INPUT {
    float2 texcoord_0 : TEXCOORD0_centroid;			// partial precision
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_3 : TEXCOORD3_centroid;			// partial precision
    float3 texcoord_4 : TEXCOORD4_centroid;			// partial precision
    float3 texcoord_5 : TEXCOORD5_centroid;			// partial precision
    float color_0 : COLOR0;
    float4 texcoord_7 : TEXCOORD7_centroid;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

#define tint yellow

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 normal0 = tex2D(NormalMap[0], IN.texcoord_0.xy);    
    float3 texture0 = tex2D(BaseMap[0], IN.texcoord_0.xy).rgb;

    float3 tangent = normalize(IN.texcoord_3.xyz);
    float3 binormal = normalize(IN.texcoord_4.xyz);
    float3 normal = normalize(IN.texcoord_5.xyz);
    float3x3 tbn = float3x3(tangent, binormal, normal);

    float3 baseColor = texture0 * IN.color_0.r;
    float3 combinedNormal = normalize(expand(normal0.xyz)) * IN.color_0.r;
    
    float3 lighting = getSunLighting(tbn, PSLightDir.xyz, PSLightColor[0].rgb, IN.texcoord_7.xyz, combinedNormal, AmbientColor.rgb);

    // Apply fog
    // float3 finalColor = (IN.texcoord_7.w * (IN.texcoord_7.xyz - (IN.texcoord_1.xyz * lighting * baseColor))) + ( lighting * baseColor * IN.texcoord_1.xyz);
    // float3 finalColor = lighting * baseColor * IN.texcoord_1.rgb;
    float3 finalColor = getFinalColor(lighting, baseColor, IN.texcoord_1.rgb);

    OUT.color_0.rgb = finalColor;
    OUT.color_0.a = 1;

    return OUT;
};


// approximately 31 instruction slots used (2 texture, 29 arithmetic)
