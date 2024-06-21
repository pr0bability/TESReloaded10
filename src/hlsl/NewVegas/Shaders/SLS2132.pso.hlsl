//  Terrain shader with blending of 6 textures
//
// Parameters:
sampler2D BaseMap[7] :register(s0);
sampler2D NormalMap[7] :register(s7);

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
float4 PSLightDir : register(c18);
float4 TESR_DebugVar : register(c27);
float4 TESR_CameraPosition : register (c28);



// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   PSLightDir   const_18      1
//   BaseMap      texture_0       6
//   NormalMap    texture_7       6
//


// Structures:

struct VS_INPUT {
    float2 texcoord_0 : TEXCOORD0;			// partial precision
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float3 texcoord_3 : TEXCOORD3_centroid;			// partial precision
    float3 texcoord_4 : TEXCOORD4_centroid;			// partial precision
    float3 texcoord_5 : TEXCOORD5_centroid;			// partial precision
    float4 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float4 texcoord_7 : TEXCOORD7_centroid;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

#define tint magenta

#include "includes/Helpers.hlsl"
#include "includes/Terrain.hlsl"

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 normal0 = tex2D(NormalMap[0], IN.texcoord_0.xy);
    float3 normal1 = tex2D(NormalMap[1], IN.texcoord_0.xy);
    float3 normal2 = tex2D(NormalMap[2], IN.texcoord_0.xy);
    float3 normal3 = tex2D(NormalMap[3], IN.texcoord_0.xy);
    float3 normal4 = tex2D(NormalMap[4], IN.texcoord_0.xy);
    float3 normal5 = tex2D(NormalMap[5], IN.texcoord_0.xy);

    float4 texture0 = tex2D(BaseMap[0], IN.texcoord_0.xy);
    float4 texture1 = tex2D(BaseMap[1], IN.texcoord_0.xy);
    float4 texture2 = tex2D(BaseMap[2], IN.texcoord_0.xy);
    float4 texture3 = tex2D(BaseMap[3], IN.texcoord_0.xy);
    float4 texture4 = tex2D(BaseMap[4], IN.texcoord_0.xy);
    float4 texture5 = tex2D(BaseMap[5], IN.texcoord_0.xy);

    float3 tangent = normalize(IN.texcoord_3.xyz);
    float3 binormal = normalize(IN.texcoord_4.xyz);
    float3 normal = normalize(IN.texcoord_5.xyz);
    float3x3 tbn = float3x3(tangent, binormal, normal);

    float3 baseColor = IN.color_0.r * texture0 + IN.color_0.g * texture1 + IN.color_0.b * texture2 + IN.color_0.a * texture3 + IN.color_1.r * texture4 + IN.color_1.g * texture5;
    float3 combinedNormal = normalize(expand(normal0) * IN.color_0.r + expand(normal1) * IN.color_0.g + expand(normal2) * IN.color_0.b + expand(normal3) * IN.color_0.a + expand(normal4) * IN.color_1.r + expand(normal5) * IN.color_1.g);

    float3 lighting = getSunLighting(tbn, PSLightDir.xyz, PSLightColor[0].rgb, IN.texcoord_7.xyz, combinedNormal, AmbientColor.rgb);

    // apply fog
    // float3 finalColor = (IN.texcoord_7.w * (IN.texcoord_7.xyz - (IN.texcoord_1.xyz * lighting * baseColor))) + (lighting * baseColor * IN.texcoord_1.xyz);
    float3 finalColor = getFinalColor(lighting, baseColor, IN.texcoord_1.rgb);

    OUT.color_0.a = 1;
    OUT.color_0.rgb = finalColor;

    return OUT;
};

// approximately 61 instruction slots used (12 texture, 49 arithmetic)